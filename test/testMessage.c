#include "app/message.h"
#include "thirdparty/log/log.h"
#include <assert.h>
#include <string.h>

int main() {
    unsigned char data[] = {0x01, 0x02, 0x04, 0x00, 0x01, 0xEA, 0xAC, 0x22, 0x88};
    char json_str[1024];
    Message messsage;
    appBinaryToMessage(data, &messsage, sizeof(data));
    appMessageToJson(&messsage, json_str, sizeof(json_str));
    log_info("%s", json_str);

    Message messsage2;
    unsigned char result[9];
    appJsonToMessage(json_str, &messsage2, sizeof(json_str));
    appMessageToBinary(&messsage2, result, sizeof(result));
    assert(memcmp(data, result, 9) == 0);

    appFreeMessage(&messsage);
    appFreeMessage(&messsage2);
}