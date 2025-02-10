#include "buffer.h"
#include "thirdparty/log/log.h"
#include <stdlib.h>
#include <string.h>

int appBufferInit(Buffer *buffer, int size)
{
    buffer->ptr = (char *)malloc(size);
    if (buffer->ptr == NULL)
    {
        log_error("malloc failed");
        return -1;
    }
    buffer->len = 0;
    buffer->size = size;
    buffer->start = 0;
    if (pthread_mutex_init(&buffer->mutex, NULL) != 0)
    {
        log_error("mutex init failed");
        free(buffer->ptr);
        return -1;
    }
    log_debug("buffer init success, size=%d", size);
    return 0;
}

int appBufferRead(Buffer *buffer, char *data, int len)
{
    if (len > buffer->len)
    {
        log_error("read len is too large");
        return -1;
    }
    pthread_mutex_lock(&buffer->mutex);
    if (len + buffer->start > buffer->size)
    {
        // 数据跨越buffer边界, 需要拆分读取
        int first_len = buffer->size - buffer->start;
        memcpy(data, buffer->ptr + buffer->start, first_len);
        memcpy(data + first_len, buffer->ptr, len - first_len);
    }
    else
    {
        // 数据不跨越buffer边界, 直接读取
        memcpy(data, buffer->ptr + buffer->start, len);
    }
    buffer->start = (buffer->start + len) % buffer->size;
    buffer->len -= len;
    pthread_mutex_unlock(&buffer->mutex);
    log_debug("read data success, len=%d", len);
    return len;
}

int appBufferWrite(Buffer *buffer, char *data, int len)
{
    if (len + buffer->len > buffer->size)
    {
        log_error("write len is too large");
        return -1;
    }
    pthread_mutex_lock(&buffer->mutex);
    int end = (buffer->start + buffer->len) % buffer->size;
    if(end + len <= buffer->size){
        // 数据不跨越buffer边界, 直接写入
        memcpy(buffer->ptr + end, data, len);
    }
    else{
        // 数据跨越buffer边界, 需要拆分写入
        int first_len = buffer->size - end;
        memcpy(buffer->ptr + end, data, first_len);
        memcpy(buffer->ptr, data + first_len, len - first_len);
    }
    buffer->len += len;
    pthread_mutex_unlock(&buffer->mutex);
    log_debug("write data success, len=%d", len);
    return 0;
}

int appBufferDestroy(Buffer *buffer)
{
    pthread_mutex_destroy(&buffer->mutex);
    if(buffer->ptr!= NULL){
        free(buffer->ptr);
        buffer->ptr = NULL;
    }
    buffer->len = 0;
    buffer->size = 0;
    buffer->start = 0;
    log_debug("buffer destroy success");
    return 0;
}
