#include "runner.h"
#include"serial.h"
#include"bluetooth.h"
#include "task.h"
#include"router.h"
#include "signal.h"

void appStop(int sig)
{
    if(sig == SIGINT || sig == SIGTERM){
        appRouterClose();
        appTaskCancel();
    }
}

void appRun(){
    signal(SIGINT, appStop);
    signal(SIGTERM, appStop);
    // 初始化线程池
    appTaskInit(5);
    // 初始化串口
    SerialDevice serialDevice;
    appSerialInit(&serialDevice, "/dev/ttyUSB0");
    // 初始化蓝牙设备
    appBluetoothInit(&serialDevice);
    // 初始化路由器
    appRouterInit();
    // 注册蓝牙设备到路由器
    appRouterRegisterDevice((Device*)&serialDevice);
    // 等待线程池结束
    appTaskWait();
}
