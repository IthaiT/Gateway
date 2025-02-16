#include "bluetooth.h"
#include "thirdparty/log/log.h"
#include <unistd.h>
#include <string.h>

static unsigned char readBuffer[128];
static int readBufferLen = 0;

static const unsigned char fixHeader[] = {0xF1, 0xDD};

/**
 * @brief 将缓冲区中前n个字节移除
 * 
 * @param n 要移除的字节数
 * @return void
 */
static void bluetoothIgnoreBuffer(int n){
    readBufferLen -= n;
    memmove(readBuffer, readBuffer + n, readBufferLen);
}

/**
 * @brief 等待ACK响应
 *
 * @param serialDevice 串口设备
 * @return int 0:成功 -1:失败
 */
static int bluetoothWaitACK(SerialDevice * serialDevice)
{
    // 等待200ms
    usleep(200000);
    unsigned char buf[4];
    read(serialDevice->super.fd, buf, 4);
    if (memcmp(buf, "OK\r\n", 4) == 0)
    {
        return 0;
    }
    return -1;
}

int appBluetoothInit(SerialDevice *serialDevice)
{
    // 设置连接类型
    serialDevice->super.connectionType = CONNECTION_TYPE_BLE;
    serialDevice->super.deviceVTable->preRead = bluetoothPreRead;
    serialDevice->super.deviceVTable->preWrite = bluetoothPreWrite;

    appSerialSetBaudRate(serialDevice, SERIAL_BAUD_RATE_9600);
    appSerialSetBlockMode(serialDevice, 0);
    appSerialFlush(serialDevice);
    if(appBluetoothStatus(serialDevice)==0){
        if(appBluetoothSetBaudRate(serialDevice, SERIAL_BAUD_RATE_115200)<0){
            log_error("appBluetoothSetBaudRate failed");
            return -1;
        }
        if(appBluetoothSetMAddr(serialDevice, "0001")<0){
            log_error("appBluetoothSetMAddr failed");
            return -1;
        }
        if(appBluetoothSetNetID(serialDevice, "1111")<0){
            log_error("appBluetoothSetNetID failed");
            return -1;
        }
        if(appBluetoothReset(serialDevice)<0){
            log_error("appBluetoothReset failed");
            return -1;
        }
    }
    appSerialSetBaudRate(serialDevice, SERIAL_BAUD_RATE_115200);
    appSerialSetBlockMode(serialDevice, 0);
    // 等待1s，抛除无效数据
    sleep(1);
    appSerialFlush(serialDevice);
    log_info("appBluetoothInit: %s successfully initialized", serialDevice->super.filename);
    return 0;
}

int appBluetoothSetBaudRate(SerialDevice *serialDevice, SerialBaudRate baudRate)
{
    char buf[] = "AT+BAUD8\r\n";
    buf[7] = baudRate;
    write(serialDevice->super.fd, buf, strlen(buf) - 1);
    return bluetoothWaitACK(serialDevice);
}

int appBluetoothSetMAddr(SerialDevice *serialDevice, char *mAddr)
{
    char buf[] = "AT+MADDR0001\r\n";
    memcpy(buf + 8, mAddr, 4);
    write(serialDevice->super.fd, buf, 14);
    return bluetoothWaitACK(serialDevice);
}

int appBluetoothSetNetID(SerialDevice *serialDevice, char *netID){
    char buf[] = "AT+NETID1111\r\n";
    memcpy(buf + 8, netID, 4);
    write(serialDevice->super.fd, buf, 14);
    return bluetoothWaitACK(serialDevice);
}

int appBluetoothReset(SerialDevice *serialDevice)
{
    write(serialDevice->super.fd, "AT+RESET\r\n", 10);
    return bluetoothWaitACK(serialDevice);
}

int appBluetoothStatus(SerialDevice *serialDevice)
{
    write(serialDevice->super.fd, "AT\r\n", 4);
    return bluetoothWaitACK(serialDevice);
}

int bluetoothPreWrite(Device *device, void *data, int len)
{
    memcpy(readBuffer + readBufferLen, data, len);
    readBufferLen += len;
    if (readBufferLen < 4)
    {
        //没有形成完整的数据包
        return -1;
    }
    for(int i = 0; i < readBufferLen - 3; i++){
        if(memcmp(readBuffer + i,"OK\r\n", 4) == 0){
            bluetoothIgnoreBuffer(4);
            return 0;
        }
        else if(memcmp(readBuffer + i, fixHeader, 2) == 0){
            bluetoothIgnoreBuffer(i);
            if(readBufferLen<readBuffer[2]+3){
                // 目前数据帧还不完整
                return -1;
            }
            // 解析数据
            // 首先写入连接类型
            memcpy(data, &device->connectionType, 1);
            // 写入id长度
            int temp = 2;
            memcpy(data + 1, &temp, 1);
            // 写入数据长度
            temp = readBuffer[2] - 4;
            memcpy(data + 2, &temp, 1);
            // 写入peer地址
            memcpy(data + 3, readBuffer + 3, 2);
            // 写入数据
            memcpy(data + 5, readBuffer + 7, readBuffer[2] - 4);
            return 0;
        }
    }
    //没找到任何想要的数据帧
    return 0;
}

int bluetoothPreRead(Device *device, void *data, int len)
{
    int temp = 0;
    unsigned char buf[30];
    // 读取连接类型
    memcpy(&temp, data, 1);
    if (temp != CONNECTION_TYPE_BLE)
    {
        return 0;
    }

    // 读取ID长度
    memcpy(&temp, data + 1, 1);
    if (temp != 2)
    {
        return 0;
    }
    // 首先拼头部
    memcpy(buf, "AT+MESH", 8);
    memcpy(buf + 8, data + 3, 2);

    // 读取数据长度
    memcpy(&temp, data + 2, 1);
    memcpy(buf + 10, data + 5, temp);
    memcpy(buf + 10 + temp, "\r\n", 2);
    len = temp + 12;
    memcpy(data, buf, len);
    return len;
}
