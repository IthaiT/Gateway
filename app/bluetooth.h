#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "serial.h"
/**
 * @brief 初始化蓝牙设备
 *
 * @param serialDevice 串口设备
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothInit(SerialDevice *serialDevice);

/**
 * @brief 设置蓝牙设备波特率
 *
 * @param serialDevice 串口设备
 * @param baudRate 波特率
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothSetBaudRate(SerialDevice *serialDevice, SerialBaudRate baudRate);

/**
 * @brief 设置蓝牙掩码
 *
 * @param serialDevice 串口设备
 * @param mAddr 掩码
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothSetMAddr(SerialDevice *serialDevice, char *mAddr);

/**
 * @brief 设置蓝牙网络ID
 *
 * @param serialDevice 串口设备
 * @param netID 网络ID
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothSetNetID(SerialDevice *serialDevice, char *netID);

/**
 * @brief 重置蓝牙设备
 *
 * @param serialDevice 串口设备
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothReset(SerialDevice *serialDevice);

/**
 * @brief 获取蓝牙设备状态
 *
 * @param serialDevice 串口设备
 * @return int 0表示成功，-1表示失败
 */
int appBluetoothStatus(SerialDevice *serialDevice);

/**
 * @brief 发送数据到蓝牙设备,将device发送过来的二进制数据转化为满足蓝牙协议的字符串数据
 *
 * @param serialDevice 串口设备
 * @param data 待发送数据
 * @param len 待发送数据长度
 * @return int 0表示成功，-1表示失败
 */
int bluetoothPreWrite(Device *device, void *data, int len);

/**
 * @brief 接收蓝牙设备发送过来的数据,将蓝牙协议的字符串数据转化为二进制数据
 *
 * @param serialDevice 串口设备
 * @param data 接收数据缓冲区
 * @param len 接收数据长度
 * @return int -1表示失败，其他表示读取的长度
 */
int bluetoothPreRead(Device *device, void *data, int len);
#endif