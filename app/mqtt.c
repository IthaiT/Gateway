#include "mqtt.h"
#include "MQTTClient.h"
#include "thirdparty/log/log.h"
#include "assert.h"
#include <stdlib.h>

static MQTTClient client; // MQTT客户端
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;// MQTT连接选项
static MQTTClient_deliveryToken token; // MQTT消息token
static int (*recvCallback)(char*, int);// MQTT消息接收回调函数
/**
 * @brief 连接丢失回调函数
 * 
 * @param context 回调上下文
 * @param cause 丢失原因
 */
void conn_lost(void *context, char *cause) {
    assert(context == NULL);
    log_error("MQTT connection lost: %s", cause);
    exit(EXIT_FAILURE);
}
/**
 * @brief 消息接收回调函数
 * 
 * @param context 回调上下文
 * @param topicName 主题名称
 * @param topicLen 主题长度
 * @param message 消息
 * @return int 0表示成功，其他表示失败
 */
int message_received(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    assert(context == NULL);
    log_debug("MQTT message received: %s, topic: %s, len: %d", (char*)message->payload, topicName, message->payloadlen);
    int result = recvCallback((char*)message->payload, message->payloadlen);
    return result;
}

/**
 * @brief 消息发送完成回调函数
 * 
 * @param context 回调上下文
 * @param token 消息token
 * @return void
 */
void delivery_complete(void *context, MQTTClient_deliveryToken token) {
    assert(context == NULL);
    log_trace("MQTT delivery complete: token %d", token);
}

int appMQTTInit(){
    int rc;
    // 创建MQTT客户端
    rc = MQTTClient_create(&client, MQTT_SERVER, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if(rc!= MQTTCLIENT_SUCCESS){
        log_error("Failed to create MQTT client: %d", rc);
        return -1;
    }
    // 设置回调函数
    rc = MQTTClient_setCallbacks(client, NULL, conn_lost, message_received, delivery_complete);
    if(rc!= MQTTCLIENT_SUCCESS){
        log_error("Failed to set MQTT callbacks: %d", rc);
        MQTTClient_destroy(&client);
        return -1;
    }
    // 设置连接选项
    conn_opts.keepAliveInterval = 20;// 每隔20秒发送心跳包
    conn_opts.cleansession = 1;// 连接是否清除之前的会话
    // 连接MQTT服务器
    rc = MQTTClient_connect(client, &conn_opts);
    if(rc!= MQTTCLIENT_SUCCESS){
        log_error("Failed to connect to MQTT server: %d", rc);
        MQTTClient_destroy(&client);
        return -1;
    }
    // 订阅主题
    rc = MQTTClient_subscribe(client, PULL_TOPIC, QOS);
    if(rc != MQTTCLIENT_SUCCESS){
        log_error("Failed to subscribe to topic: %d", rc);
        MQTTClient_disconnect(client, 1000);
        MQTTClient_destroy(&client);
        return -1;
    }
    log_info("MQTT client initialized successfully");
    return 0;
}

int appMQTTSend(char *jsonstr, int len){
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = jsonstr;
    message.payloadlen = len;
    message.qos = QOS;
    message.retained = 0;
    int rc = MQTTClient_publishMessage(client, PUSH_TOPIC, &message, &token);
    if(rc != MQTTCLIENT_SUCCESS){
        log_error("Failed to publish message: %d", rc);
        return -1;
    }
    log_debug("MQTT message sent: %s, len: %d", jsonstr, len);
    return 0;
}

void appMQTTRecvCallback(int (*callback)(char *, int)){
    recvCallback = callback;
}

int appMQTTClose(){
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    log_info("MQTT client closed successfully");
}