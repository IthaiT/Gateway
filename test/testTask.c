#include "app/task.h"
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void task_func(void *arg){
    pthread_mutex_lock(&lock);
    (*(int *)arg)++;
    pthread_mutex_unlock(&lock);
}

int main(){
    appTaskInit(8);
    int a = 100;
    for(int i = 0; i < 100; i++){
        appTaskRegister(task_func, &a);
    }
    sleep(5);
    assert(a == 200);
    appTaskCancel();
    appTaskWait();
    return 0;
}