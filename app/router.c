#include "router.h"
#include "mqtt.h"
#include "message.h"
#include "thirdparty/log/log.h"
#include <string.h>

#define MAX_DEVICES 10

Device *devices[MAX_DEVICES];
static int num_devices = 0;
int appMQTTCallback(char *json_str, int len);
int appDeviceCallback(void *binary_data, int len);

int appRouterInit()
{
    //开启MQTT客户端
    if(appMQTTInit()<0){
        return -1;
    }
    //注册MQTT消息接收回调函数
    appMQTTRecvCallback(appMQTTCallback);
    return 0;
}

int appRouterRegisterDevice(Device *device){
    if(num_devices>=MAX_DEVICES){
        return -1;
    }
    devices[num_devices] = device;
    num_devices++;
    //注册设备数据接收回调函数
    appDeviceReceiveCallback(device, appDeviceCallback);
    //启动设备
    appDeviceStart(device);
    return 0;
}

int appRouterClose(){
    for(int i=0;i<num_devices;i++){
        appDeviceClose(devices[i]);
        appDeviceStop(devices[i]);
    }
    appMQTTClose();
}
/**
 * @brief 处理MQTT消息的回调函数,MQTT消息发送到路由，路由将其转发到对应设备
 * 
 * @param json_str 接收到的MQTT消息的json格式数据
 * @param len 接收到的MQTT消息的长度
 * @return int 0表示成功，-1表示失败
 */
int appMQTTCallback(char* json_str, int len){
    //将json格式数据转化为二进制数据
    Message message;
    if(appJsonToMessage(json_str, &message, len)<0){
        return -1;
    }
    char binary_data[1024];
    int binary_len = appMessageToBinary(&message, binary_data, sizeof(binary_data));
    if(binary_len<0){
        return -1;
    }
    //将二进制数据转发给对应的设备
    for(int i=0;i<num_devices;i++){
        if(devices[i]->connectionType == message.connectionType){
            appDeivceWrite(devices[i], binary_data, binary_len);
        }
    }
    appFreeMessage(&message);
    return 0;
}

/**
 * @brief 处理设备收到下层数据时的回调函数，将数据转发给MQTT服务器
 * 
 * @param data 收到的device传递上来的二进制数据
 * @param len 收到的数据长度
 * @return int 0表示成功，-1表示失败
 */
int appDeviceCallback(void* binary_data, int len){
    //将下层传递上来的二进制数据转化为json格式数据
    Message message;
    if(appBinaryToMessage((char*)binary_data, &message, len)<0){
        return -1;
    }
    char json_data[1024];
    int json_len = appMessageToJson(&message, json_data, sizeof(json_data));
    if(json_len<0){
        return -1;
    }
    //将json格式数据发送给MQTT服务器
    appMQTTSend(json_data, json_len);
    appFreeMessage(&message);
    return 0;
}
