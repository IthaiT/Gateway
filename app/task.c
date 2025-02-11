#include "task.h"
#include <pthread.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include "thirdparty/log/log.h"

#define MAX_TASK_NUM 10
#define MSG_LEN sizeof(struct TaskStruct)

struct TaskStruct
{
    Task task;
    void *arg;
};

static pthread_t* task_threads;
static mqd_t task_queue;
static int thread_num = 0;

static void *task_executer(void *arg){
    struct TaskStruct taskStruct;
    log_info("Thread %d started", *(int*)arg);
    while (1)
    {
        if(mq_receive(task_queue, (char*)&taskStruct, MSG_LEN, NULL) == -1){
            continue;
        }
        taskStruct.task(taskStruct.arg);
    }
}

int appTaskInit(int num){
    // 创建消息队列
    struct mq_attr attr;
    attr.mq_maxmsg = MAX_TASK_NUM;
    attr.mq_msgsize = MSG_LEN;
    task_queue = mq_open("/gateway_mqueue", O_CREAT | O_RDWR, 0644, &attr);
    if(task_queue == -1){
        log_error("Failed to create task queue");
        return -1;
    }
    // 初始化线程池
    thread_num = num;
    task_threads = (pthread_t *)malloc(num * sizeof(pthread_t));
    if(task_threads == NULL){
        log_error("Failed to allocate memory for task threads");
        mq_unlink("/gateway-mqueue");
        return -1;
    }
    memset(task_threads, 0, num * sizeof(pthread_t));
    // 创建线程
    for (int i = 0; i < thread_num; i++)
    {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&task_threads[i], NULL, task_executer, id) != 0)
        {
            log_error("Failed to create task thread %d", i);
            for(int j = 0; j < i; j++){
                pthread_cancel(task_threads[j]);
                pthread_join(task_threads[j], NULL);
            }
            free(task_threads);
            mq_unlink("/gateway_mqueue");
            return -1;
        }
    }
    log_info("Task pool initialized with %d threads", thread_num);
    return 0;
}

int appTaskRegister(Task task, void *arg){
    struct TaskStruct taskStruct;
    taskStruct.task = task;
    taskStruct.arg = arg;
    if(mq_send(task_queue, (char*)&taskStruct, MSG_LEN, 0) == -1){
        log_error("Failed to send task to queue");
        return -1;
    }
    return 0;
}

int appTaskWait(){
    for(int i = 0; i < thread_num; i++){
        if(task_threads[i]!= 0){
            pthread_join(task_threads[i], NULL);
        }
    }
    mq_unlink("/gateway_mqueue");
    log_info("all tasks Completed");
    return 0;
}

int appTaskCancel(){
    log_info("Canceling all tasks");
    for(int i = 0; i < thread_num; i++){
        if(task_threads[i]!= 0){
            pthread_cancel(task_threads[i]);
        }
    }
    return 0;
}
