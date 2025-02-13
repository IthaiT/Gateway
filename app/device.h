#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "mqtt.h"
#include "message.h"
#include "task.h"
#include "buffer.h"
#include <pthread.h>

typedef struct DeviceVTableStruct DeviceVTable;

typedef struct DeviceStruct{
    DeviceVTable *deviceVTable;     // 设备的虚表
    char *filename;                 // 文件名
    int fd;                         // 文件描述符
    ConnectionType connectionType;  // 连接类型
    Buffer *sendBuffer;               // 发送缓冲区
    Buffer *receiveBuffer;            // 接收缓冲区
    int isrunning;                  // 是否运行
    pthread_t backgroundThread;     // 后台线程
} Device;

typedef struct DeviceVTableStruct
{
    void* (*backgroundThreadFunc)(void *args); // 后台线程函数
    Task sendTask;// 发送任务,用于发送消息到设备
    Task receiveTask;// 接收任务,用于从设备中接受消息
    int (*preWrite)(Device *device, void *data, int len);// 预写函数,用于处理数据
    int (*preRead)(Device *device, void *data, int len); // 预读函数,用于处理数据
    int (*receiveCallback)(void *data, int len);         // 接收回调函数,用于处理接收到的数据
} DeviceVTable;

/**
 * @brief 初始化设备
 * 
 * @param device 设备
 * @param filename 设备文件名
 * @param connectionType 连接类型
 * @return int 0表示成功，-1表示失败
 */
int appDeviceInit(Device *device, char *filename, ConnectionType connectionType);
/**
 * @brief 启动设备
 * 
 * @param device 设备
 * @return int 0表示成功，-1表示失败
 */
int appDeviceStart(Device *device);
/**
 * @brief 停止设备
 * 
 * @param device 设备
 * @return int 0表示成功，-1表示失败
 */
void appDeviceStop(Device *device);
/**
 * @brief 发送消息到设备
 * 
 * @param device 设备
 * @return int 0表示成功，-1表示失败
 */
void appDeviceClose(Device *device);

// 设备接受到消息的回调函数
void appReceiveCallback(Device *device, int (*receiveCallback)(void *data, int len));

/**
 * @brief 往设备中写入数据
 * 
 * @param device 设备
 * @param data 发送数据
 * @param len 发送数据长度
 */
int appDeivceWrite(Device *device, void *data, int len);

#endif