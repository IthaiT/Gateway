#include "device.h"
#include <stdlib.h>
#include "thirdparty/log/log.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 16384
static void defaultSendTask(void *args);
static void defaultReceiveTask(void *args);
static void *backgroundThreadFunc(void *args);

int appDeviceInit(Device *device, char *filename)
{
    // 为Device结构体分配内存
    device->deviceVTable = malloc(sizeof(DeviceVTable));
    if (device->deviceVTable == NULL)
    {
        log_error("device vtable malloc failed");
        goto Exit;
        return -1;
    }
    device->filename = malloc(strlen(filename) + 1);
    if (device->filename == NULL)
    {
        log_error("device filename malloc failed");
        goto VTableExit;
    }
    device->sendBuffer = malloc(sizeof(Buffer));
    if (device->sendBuffer == NULL)
    {
        log_error("device send buffer malloc failed");
        goto FilenameExit;
    }
    device->receiveBuffer = malloc(sizeof(Buffer));
    if (device->receiveBuffer == NULL)
    {
        log_error("device receive buffer malloc failed");
        goto SendBufferExit;
    }
    // 初始化Device结构体
    strcpy(device->filename, filename);
    device->isrunning = 0;
    device->fd = open(filename, O_RDWR | O_NOCTTY);//防止将设备打开为终端设备
    if (device->fd < 0)
    {
        log_error("device open failed");
        goto ReceiveBufferExit;
    }
    if (appBufferInit(device->sendBuffer, BUFFER_SIZE) < 0)
    {
        log_error("device send buffer init failed");
        goto DeviceOpenExit;
    }
    if (appBufferInit(device->receiveBuffer, BUFFER_SIZE) < 0)
    {
        log_error("device receive buffer init failed");
        goto SendBufferInitExit;
    }
    // TODO: 初始化DeviceVTable
    device->deviceVTable->backgroundThreadFunc = backgroundThreadFunc;
    device->deviceVTable->sendTask = defaultSendTask;
    device->deviceVTable->receiveTask = defaultReceiveTask;
    device->deviceVTable->preWrite = NULL;
    device->deviceVTable->preRead = NULL;
    device->deviceVTable->receiveCallback = NULL;
    log_debug("device %s init success", device->filename);
    return 0;
ReveiveBufferInitExit:
    appBufferDestroy(device->receiveBuffer);
SendBufferInitExit:
    appBufferDestroy(device->sendBuffer);
DeviceOpenExit:
    close(device->fd);
ReceiveBufferExit:
    free(device->receiveBuffer);
SendBufferExit:
    free(device->sendBuffer);
FilenameExit:
    free(device->filename);
VTableExit:
    free(device->deviceVTable);
Exit:
    return -1;
}

int appDeviceStart(Device *device)
{
    if (device->isrunning == 1)
    {
        return -1;
    }
    if (pthread_create(&(device->backgroundThread), NULL, device->deviceVTable->backgroundThreadFunc, device))
    {
        log_error("device background thread create failed");
        return -1;
    }
    log_info("device %s start success", device->filename);
    device->isrunning = 1;
    return 0;
}

void appDeviceStop(Device *device)
{
    if (device->isrunning == 0)
    {
        return;
    }
    if (device->deviceVTable->backgroundThreadFunc != NULL)
    {
        pthread_cancel(device->backgroundThread);
        pthread_join(device->backgroundThread, NULL);
    }
    device->isrunning = 0;
}

void appDeviceClose(Device *device)
{
    appBufferDestroy(device->receiveBuffer);
    appBufferDestroy(device->sendBuffer);
    close(device->fd);
    free(device->receiveBuffer);
    free(device->sendBuffer);
    free(device->filename);
    free(device->deviceVTable);
}

void appDeviceReceiveCallback(Device *device, int (*receiveCallback)(void *data, int len))
{
    device->deviceVTable->receiveCallback = receiveCallback;
}

int appDeivceWrite(Device *device, void *data, int len)
{
    // 首先将数据写入接受缓冲中
    if (appBufferWrite(device->sendBuffer, data, len) < 0)
    {
        log_error("device write failed in buffer write");
        return -1;
    }
    // 注册一个写缓冲区函数，将数据写入设备
    appTaskRegister(device->deviceVTable->sendTask, device);
    return 0;
}

/**
 * @brief 内部函数，设备后台线程，不断地读取设备数据，写入缓存区，然后注册接受任务
 */
static void *backgroundThreadFunc(void *args)
{
    Device *device = (Device *)args;
    char buf[1024];
    while (device->isrunning == 1)
    {
        // 不断读取设备数据
        int len = read(device->fd, buf, 1024);
        if (len < 0)
        {
            log_error("device read failed in background thread");
            continue;
        }
        // 预处理设备数据（对每一种设备单独处理），转化为2进制数据
        if (device->deviceVTable->preRead != NULL)
        {
            len = device->deviceVTable->preRead(device, buf, len);
        }
        // 将处理完的数据写入缓冲区
        if (len > 0)
        {
            appBufferWrite(device->receiveBuffer, buf, len);
        }
        // 注册一个接受函数，将接受到的二进制消息发送给mqtt服务器
        appTaskRegister(device->deviceVTable->receiveTask, device);
    }
}

/**
 * @brief 内部函数，默认的接收任务，从接收缓冲区中读取数据，并调用接收回调函数处理数据
 *
 * @param device 设备结构体
 */
static void defaultReceiveTask(void *args)
{
    Device *device = (Device *)args;
    // 从接收缓冲区中读取数据
    char buf[1024];
    appBufferRead(device->receiveBuffer, buf, 3);
    int len = buf[1] + buf[2];
    appBufferRead(device->receiveBuffer, buf, len);
    int buf_len = len + 3;
    while (device->deviceVTable->receiveCallback(buf, buf_len) < 0)
    { // 回调函数，上层决定如何处理数据
        usleep(100000);
    }
}

/**
 * @brief 内部函数，默认的发送任务，从发送缓冲区中读取数据，并调用写前处理函数处理数据，然后写入设备
 *
 * @param device 设备结构体
 */
static void defaultSendTask(void *args)
{
    Device *device = (Device *)args;
    char buf[1024];
    // 从发送缓冲区读取数据
    appBufferRead(device->sendBuffer, buf, 3);
    int len = buf[1] + buf[2];
    appBufferRead(device->sendBuffer, buf, len);
    int buf_len = len + 3;
    // 调用写前处理函数
    if (device->deviceVTable->preWrite != NULL)
    {
        buf_len = device->deviceVTable->preWrite(device, buf, buf_len);
    }
    if (buf_len < 0)
    {
        log_error("device pre write failed in pre write");
    }
    // 将数据写入设备
    if (write(device->fd, buf, buf_len) < 0)
    {
        log_error("device write failed in send task");
    }
}
