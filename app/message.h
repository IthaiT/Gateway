#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/**
 * 二进制数据格式
 * 第一个字节：连接类型（1字节）
 * 第二个字节：设备ID长度（1字节）
 * 第三个字节：数据长度（1字节）
 * 第四个字节：设备ID+数据
 * 
 * JSON格式：
 * {
 *     "connectionType": 1,
 *     "id": "0x123",
 *     "data": "0xabc"
 * }
 * message.payload存储的是二进制数据，转化为json将16进制直接转化为字符串
 * 
 */

// 连接类型（哪种设备连接）
typedef enum ConnetionTypeEnum
{
    CONNECTION_TYPE_NONE = 0, // 未知设备
    CONNECTION_TYPE_LORA = 1, // LoRa设备
    CONNECTION_TYPE_BLE = 2,  // BLE设备
} ConnetionType;

// 消息结构体
typedef struct message_t
{
    ConnetionType connectionType;
    char *payload; // 真实数据
    int id_len;    // 设备ID长度
    int data_len;  // 数据长度
} Message;

/**
 * @brief 解析json字符串到Message结构体
 *
 * @param json 待解析的json字符串
 * @param message 解析结果存放的Message结构体
 * @param json_len json字符串长度
 * @return 0:成功, -1:失败
 */
int appJsonToMessage(char *json, Message *message, int json_len);
/**
 * @brief 将Message结构体转换为json字符串
 *
 * @param message 待转换的Message结构体
 * @param json 转换结果存放的json字符串
 * @param json_len json字符串长度
 * @return -1:失败, total_len:具体转换的json字符串长度
 */
int appMessageToJson(Message *message, char *json, int json_len);
/**
 * @brief 将二进制数据转化为Message结构体
 *
 * @param binary 待转换的二进制数据
 * @param message 转换结果存放的Message结构体
 * @param binary_len 二进制数据长度
 * @return 0:成功, -1:失败
 */
int appBinaryToMessage(char *binary, Message *message, int binary_len);
/**
 * @brief 将Message结构体转换为二进制数据
 *
 * @param message 待转换的Message结构体
 * @param binary 转换结果存放的二进制数据
 * @param binary_len 二进制数据长度
 * @return -1:失败, total_len:具体转换的二进制数据长度
 */
int appMessageToBinary(Message *message, char *binary, int binary_len);
/**
 * @brief 释放Message结构体
 *
 * @param message 待释放的Message结构体
 * @return 0:成功, -1:失败
 */
int appFreeMessage(Message *message);
#endif