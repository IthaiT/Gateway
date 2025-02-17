#ifndef __DAEMON_PROCESS_H__
#define __DAEMON_PROCESS_H__

#include <sys/types.h>

#define PROGRAM_NAME "/usr/bin/gateway"

typedef struct SubProcessStruct
{
    pid_t pid;
    char *name;
    char** args;
} SubProcess;

/**
 * @brief 初始化子进程（ota进程与main进程）
 * 
 * @param subProcess 子进程结构体
 * @param name 子进程名称
 * @return int 0：成功，-1：失败
 */
int subProcessInit(SubProcess *subProcess, char *name);

/**
 * @brief 启动子进程
 * 
 * @param subProcess 子进程结构体
 * @return int 0：成功，-1：失败
 */
int subProcessStart(SubProcess *subProcess);

/**
 * @brief 停止子进程
 * 
 * @param subProcess 子进程结构体
 * @return int 0：成功，-1：失败
 */
int subProcessStop(SubProcess *subProcess);
#endif