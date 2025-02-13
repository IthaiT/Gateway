#ifndef __ROUTER_H__
#define __ROUTER_H__

#include "device.h"

/**
 * @brief 初始化路由器
 * 
 * @return int 0表示成功，-1表示失败
 */
int appRouterInit();

/**
 * @brief 注册设备
 * 
 * @param device 设备对象
 * @return int 0表示成功，-1表示失败
 */

int appRouterRegisterDevice(Device *device);

/**
 * @brief 启动路由器
 * 
 * @return int 0表示成功，-1表示失败
 */
int appRouterClose();
#endif