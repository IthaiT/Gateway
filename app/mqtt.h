#ifndef __MQTT_H__
#define __MQTT_H__

#define MQTT_SERVER "tcp://localhost:1883"
#define CLIENT_ID "ithai-ubuntu-c0f6cc06be52"
#define PULL_TOPIC "PULL_TOPIC"
#define PUSH_TOPIC "PUSH_TOPIC"
#define QOS 0

/**
 * @brief mqtt初始化
 * 
 * @return int 0:成功 -1:失败
 */
int appMQTTInit();

/**
 * @brief mqtt发送消息
 * 
 * @param jsonstr 待发送的json字符串
 * @param len 待发送的json字符串长度
 * @return int 0:成功 -1:失败
 */
int appMQTTSend(char* jsonstr, int len);

/**
 * @brief mqtt接收消息回调函数
 * 
 * @param recvCallback 接收消息回调函数
 * @return void
 */
void appMQTTRecvCallback(int (*callback)(char* ,int ));

/**
 * @brief mqtt关闭连接
 * 
 * @return int 0:成功 -1:失败
 */
int appMQTTClose();
#endif