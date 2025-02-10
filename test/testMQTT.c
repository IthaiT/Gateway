#include "app/mqtt.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

int reveive_flag = 0;
int recvCallback(char* message,int len){
    reveive_flag = 1;
    assert(memcmp(message, "Hello MQTT", len) == 0);
    return 0;
}

int main() {
    appMQTTInit();
    appMQTTRecvCallback(recvCallback);
    appMQTTSend("Hello MQTT", 11);
    sleep(1);
    assert(reveive_flag == 1);
    appMQTTClose();
    return 0;
}