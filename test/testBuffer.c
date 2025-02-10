#include "app/buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
    Buffer buffer;
    appBufferInit(&buffer, 16);
    appBufferWrite(&buffer, "hello world", 11);
    int readLen = appBufferRead(&buffer, (char *)malloc(10), 6);
    assert(readLen == 6);
    appBufferWrite(&buffer, "1234567890", 10);
    char *data = (char *)malloc(15);
    appBufferRead(&buffer, data, 15);
    assert(strcmp(data, "world1234567890") == 0);
    readLen = appBufferRead(&buffer, data, 0);
    assert(readLen == 0);
    appBufferWrite(&buffer, "", 0);
    assert(buffer.len == 0);
    appBufferDestroy(&buffer);
    return 0;
}