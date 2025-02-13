#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "device.h"
#include <termios.h>

typedef enum 
{
    SERIAL_BAUD_RATE_9600 = '4',
    SERIAL_BAUD_RATE_115200 = '8',
}SerialBaudRate;

typedef enum
{
    STOP_BITS_1 = 0,
    STOP_BITS_2 = CSTOPB,
}StopBits;

typedef enum
{
    PARITY_NONE = 0,
    PARITY_ODD = (PARENB | PARODD),
    PARITY_EVEN = PARENB,
}Parity;

typedef struct SerialDeviceStruct
{
    Device super;           //继承父类
    SerialBaudRate baudRate;//波特率
    StopBits stopBits;      //停止位
    Parity parity;          //校验位
} SerialDevice;

/**
 * @brief 初始化串口设备
 * 
 * @param serialDevice 串口设备
 * @param filename 串口设备文件名
 * @return int 0表示成功，其他表示失败
 */
int appSerialInit(SerialDevice *serialDevice, char *filename);

/**
 * @brief 设置串口设备波特率
 *  
 * @param serialDevice 串口设备
 * @param baudRate 波特率
 * @return 无
 */
void appSerialSetBaudRate(SerialDevice *serialDevice, SerialBaudRate baudRate);

/**
 * @brief 设置串口设备停止位
 * 
 * @param serialDevice 串口设备
 * @param stopBits 停止位
 * @return 无
 */
void appSerialSetStopBits(SerialDevice *serialDevice, StopBits stopBits);

/**
 * @brief 设置串口设备校验位
 * 
 * @param serialDevice 串口设备
 * @param parity 校验位
 * @return 无
 */
void appSerialSetParity(SerialDevice *serialDevice, Parity parity);

/**
 * @brief 设置串口设备阻塞模式
 * 
 * @param serialDevice 串口设备
 * @param enable 1表示阻塞模式，0表示非阻塞模式
 * @return 无
 */
void appSerialSetBlockMode(SerialDevice *serialDevice, int enable);

/**
 * @brief 刷新串口设备
 * 
 * @param serialDevice 串口设备
 * @return 无
 */
void appSerialFlush(SerialDevice *serialDevice);

#endif