#include "message.h"
#include <stdlib.h>
#include <string.h>
#include "thirdparty/log/log.h"
#include "thirdparty/cJSON/cJSON.h"

/**
 * @brief 将字节数组转换为十六进制字符串
 * 
 * @param bin 字节数组
 * @param hex 十六进制字符串
 * @param len 字节数组长度
 * @return 0 成功，-1 失败
 */
int bintohex(char *bin, char *hex, int len){
    for(int i = 0; i < len; i++){
        sprintf(hex + i * 2, "%02x", (unsigned char)bin[i]);
    }
    return 0;
}
/**
 * @brief 将十六进制字符串转换为字节数组
 * 
 * @param hex 十六进制字符串
 * @param bin 字节数组
 * @param len 字节数组长度
 * @return 0 成功，-1 失败
 */
int hextobin(char *hex, char *bin, int len){
    if(strlen(hex) % 2!= 0){
        log_error("hex string length not correct");
        return -1;
    }
    if(len * 2 < strlen(hex)){
        log_error("bin length not enough");
        return -1;
    }
    for(int i = 0; i < len; i++){
        if(hex[i * 2] >= '0' && hex[i * 2] <= '9'){
            bin[i] = hex[i * 2];
        }
        else if (hex[i * 2] >= 'a' && hex[i * 2] <= 'f'){
            bin[i] = hex[i * 2] - 'a' + 10;
        }
        else if (hex[i * 2] >= 'A' && hex[i * 2] <= 'F'){
            bin[i] = hex[i * 2] - 'A' + 10;
        }
        else{
            log_error("invalid hex char error");
            return -1;
        }
        bin[i] = bin[i] << 4;
        if (hex[i * 2 + 1] >= '0' && hex[i * 2 + 1] <= '9')
        {
            bin[i] |= hex[i * 2 + 1] - '0';
        }
        else if (hex[i * 2 + 1] >= 'a' && hex[i * 2 + 1] <= 'f')
        {
            bin[i] |= hex[i * 2 + 1] - 'a' + 10;
        }
        else if (hex[i * 2 + 1] >= 'A' && hex[i * 2 + 1] <= 'F')
        {
            bin[i] |= hex[i * 2 + 1] - 'A' + 10;
        }
        else{
            log_error("invalid hex char error");
            return -1;
        }
    }
    return 0;
}

int appJsonToMessage(char *json, Message *message, int json_len)
{
    memset(message, 0, sizeof(Message));
    cJSON *root = cJSON_ParseWithLength(json, json_len);
    if (root == NULL)
    {
        log_error("parse json failed");
        return -1;
    }
    //解析connectionType
    message->connectionType = cJSON_GetObjectItem(root, "connectionType")->valueint;
    int id_len = strlen(cJSON_GetObjectItem(root, "id")->valuestring);
    if(id_len % 2!= 0){
        log_error("id length not correct");
        return -1;
    }
    //得到id的长度，并将其转换为字节数
    message->id_len = id_len / 2;
    int data_len = strlen(cJSON_GetObjectItem(root, "data")->valuestring);
    if(data_len % 2!= 0){
        log_error("data length not correct");
        return -1;
    }
    //得到data的长度，并将其转换为字节数
    message->data_len = data_len / 2;
    message->payload = (char *)malloc(message->data_len + message->id_len);
    if (message->payload == NULL){
        log_error("malloc payload failed");
        return -1;
    }
    //将id和data转换为字节数组
    if(hextobin(cJSON_GetObjectItem(root, "id")->valuestring, message->payload, message->id_len)!=0){
        log_error("id hextobin failed");
        cJSON_Delete(root);
        return -1;
    }
    if(hextobin(cJSON_GetObjectItem(root, "data")->valuestring, message->payload + message->id_len, message->data_len)!=0){
        log_error("data hextobin failed");
        cJSON_Delete(root);
        return -1;
    }
    //释放内存
    cJSON_Delete(root);
    return 0;
}

int appMessageToJson(Message *message, char *json, int json_len)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL)
    {
        log_error("create json object failed");
        return -1;
    }
    //设置connectionType
    cJSON_AddNumberToObject(root, "connectionType", message->connectionType);
    //设置id
    char id[message->id_len * 2 + 1];
    if(bintohex(message->payload, id, message->id_len)!=0){
        log_error("id bintohex failed");
        cJSON_Delete(root);
        return -1;
    }
    cJSON_AddStringToObject(root, "id", id);
    //设置data
    char data[message->data_len * 2 + 1];
    if(bintohex(message->payload + message->id_len, data, message->data_len)!=0){
        log_error("data bintohex failed");
        cJSON_Delete(root);
        return -1;
    }
    cJSON_AddStringToObject(root, "data", data);
    //将json转换为字符串

    char *json_str = cJSON_PrintUnformatted(root);
    if(json_str == NULL){
        log_error("print json failed");
        cJSON_Delete(root);
        return -1;
    }
    int json_str_len = strlen(json_str);
    if(json_str_len >= json_len){
        log_error("json_len not enough");
        cJSON_Delete(root);
        return -1;
    }
    //将json字符串复制到json缓冲区
    strncpy(json, json_str, json_str_len);
    //释放内存
    free(json_str);
    cJSON_Delete(root);
    return json_str_len;
}

int appBinaryToMessage(char *binary, Message *message, int binary_len)
{
    memset(message, 0, sizeof(Message));
    //解析connectionType, data_len, id_len
    memcpy(&message->connectionType, binary, 1);
    memcpy(&message->id_len, binary + 1, 1);
    memcpy(&message->data_len, binary + 2, 1);
    if (message->data_len + message->id_len + 3 != binary_len)
    {
        log_error("binary data length not match,binary_len:%d, data_len:%d, id_len:%d", binary_len, message->data_len, message->id_len);
        return -1;
    }
    //得到设备id与data的字节数组
    message->payload = (char *)malloc(message->data_len + message->id_len);
    if (message->payload == NULL)
    {
        log_error("malloc real_data failed");
        return -1;
    }
    memcpy(message->payload, binary + 3, message->data_len + message->id_len);
    return 0;
}

int appMessageToBinary(Message *message, char *binary, int binary_len)
{
    int total_len = message->data_len + message->id_len + 3;
    if (total_len > binary_len)
    {
        log_error("binary data length not enough");
        return -1;
    }
    binary[0] = message->connectionType;
    binary[1] = message->id_len;
    binary[2] = message->data_len;
    memcpy(binary + 3, message->payload, message->data_len + message->id_len);
    return total_len;
}

int appFreeMessage(Message *message)
{
    if (message->payload != NULL)
    {
        free(message->payload);
        message->payload = NULL;
    }
    return 0;
}