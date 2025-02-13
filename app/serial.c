#include "serial.h"
#include "thirdparty/log/log.h"
#include "message.h"

/**
 * @brief 内部函数，设置串口为8数据位
 * 
 * @param serialDevice 串口设备
 * @return 无
 */
static void appSerialSetCS8(SerialDevice *serialDevice){
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetCS8: tcgetattr error");
        return;
    }
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetCS8: tcsetattr error");
        return;
    }
}

/**
 * @brief 内部函数，设置串口为非终端设备
 * 
 * @param serialDevice 串口设备
 * @return 无
 */
static void appSerialSetRaw(SerialDevice *serialDevice){
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetRaw: tcgetattr error");
        return;
    }
    cfmakeraw(&options);
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetRaw: tcsetattr error");
        return;
    }
}

int appSerialInit(SerialDevice *serialDevice, char *filename){
    if(appDeviceInit(&serialDevice->super, filename)<0){
        return -1;
    }
    serialDevice->super.connectionType = CONNECTION_TYPE_BLE;
    appSerialSetBaudRate(serialDevice, SERIAL_BAUD_RATE_9600);
    appSerialSetStopBits(serialDevice, STOP_BITS_1);
    appSerialSetParity(serialDevice, PARITY_NONE);
    appSerialSetBlockMode(serialDevice, 0);
    appSerialSetCS8(serialDevice);
    appSerialSetRaw(serialDevice);
    appSerialFlush(serialDevice);
    log_info("appSerialInit: %s successfully initialized", filename);
    return 0;
}

void appSerialSetBaudRate(SerialDevice *serialDevice, SerialBaudRate baudRate){
    //获取当前串口设置
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetBaudRate: tcgetattr error");
        return;
    }
    //设置新的波特率
    switch(baudRate){
        case SERIAL_BAUD_RATE_9600:
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            break;
        case SERIAL_BAUD_RATE_115200:
            cfsetispeed(&options, B115200);
            cfsetospeed(&options, B115200);
            break;
        default:
            log_error("unsupported baud rate");
            return;
    }
    //设置新的串口设置
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetBaudRate: tcsetattr error");
        return;
    }
    serialDevice->baudRate = baudRate;
}

void appSerialSetStopBits(SerialDevice *serialDevice, StopBits stopBits){
    //获取当前串口设置
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetStopBits: tcgetattr error");
        return;
    }
    //设置新的停止位
    options.c_cflag &= ~CSTOPB;
    options.c_cflag |= stopBits;
    //设置新的串口设置
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetStopBits: tcsetattr error");
        return;
    }
    serialDevice->stopBits = stopBits;
}

void appSerialSetParity(SerialDevice *serialDevice, Parity parity){
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetParity: tcgetattr error");
        return;
    }
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag |= parity;
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetParity: tcsetattr error");
        return;
    }
    serialDevice->parity = parity;
}

void appSerialSetBlockMode(SerialDevice *serialDevice, int enable) {
    struct termios options;
    if(tcgetattr(serialDevice->super.fd, &options)<0){
        log_error("appSerialSetBlockMode: tcgetattr error");
        return;
    }
    if(enable){
        options.c_cc[VMIN] = 1;
        options.c_cc[VTIME] = 0;
    }else{
        options.c_cc[VMIN] = 0;
        options.c_cc[VTIME] = 5;
    }
    if (tcsetattr(serialDevice->super.fd, TCSAFLUSH, &options) < 0)
    {
        log_error("appSerialSetBlockMode: tcsetattr error");
        return;
    }
}

void appSerialFlush(SerialDevice *serialDevice) {
    tcflush(serialDevice->super.fd, TCIOFLUSH);
}
