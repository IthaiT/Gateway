#ifndef __BUFFER__H__
#define __BUFFER__H__

#include <pthread.h>

//一个环形缓冲区
typedef struct buffer_t {
    char *ptr; //指向buffer的指针
    int len; //当前buffer数据长度
    int size; //buffer的容量
    int start; //指向buffer的开始位置
    pthread_mutex_t mutex; //互斥锁
}Buffer;

/**
 * @brief 初始化buffer
 *
 * @param buffer 要初始化的buffer
 * @param szie 要初始化的buffer的长度
 * @return 返回初始化的结果，0表示初始化成功，-1表示初始化失败
 */
int appBufferInit(Buffer *buffer, int size);

/**
 * @brief 往buffer中读取数据
 * 
 * @param buffer 读取的buffer
 * @param data 读取的数据
 * @param len 读取的长度
 * @return 返回读取的长度，-1表示读取失败
 */
int appBufferRead(Buffer *buffer, char *data, int len);

/**
 * @brief 从buffer中写入数据
 * 
 * @param buffer 读取的buffer
 * @param data 写入的数据
 * @param len 写入的长度
 * @return 返回写入的长度，-1表示写入失败 0表示写入成功
 */
int appBufferWrite(Buffer *buffer, char *data, int len);

/**
 * @brief 销毁buffer
 * 
 * @param buffer 要销毁的buffer
 * @return 返回销毁的结果，0表示销毁成功，-1表示销毁失败
 */
int appBufferDestroy(Buffer *buffer);
#endif
