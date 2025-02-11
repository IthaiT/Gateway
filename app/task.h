#ifndef __TASK_H__
#define __TASK_H__

typedef void (*Task)(void *);// 任务函数指针

/**
 * @brief 初始化线程池，用于执行任务
 * 
 * @param num 线程池大小
 * @return 0 成功，-1 失败
 */
int appTaskInit(int num);

/**
 * @brief 注册任务
 * 
 * @param task 任务函数指针
 * @return 0 成功，-1 失败
 */
int appTaskRegister(Task task, void *arg);

/**
 * @brief 等待任务执行完毕
 * 
 * @return 0 成功，-1 失败
 */
int appTaskWait();

/**
 * @brief 取消任务
 * 
 * @return 0 成功，-1 失败
 */
int appTaskCancel();

#endif