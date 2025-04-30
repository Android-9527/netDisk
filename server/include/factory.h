//factory.h
#include "taskQueue.h"

#ifndef __FACTORY__
#define __FACTORY__
typedef struct {
    int clientFd;  //与客户端通讯的socket文件
    char currentPath[1024];  // 每个客户端独立的文件路径
} ClientInfo;
//这里用来描述整个进程池的信息，也是线程间共享的数据
typedef struct factory_s{
    pthread_t *tidArr;  //存储线程数组
    int threadNum;      //线程数量
    int runningFlag;    //服务端是否终止
    taskQueue_t taskQueue;  //任务结构体：任务链表，锁，信号量
} factory_t;
// 初始化factor，互斥，信号量
int factoryInit(factory_t *pFactory, int threadNum);
// 初始化进程添加线程pid到factory中，执行线程函数
void makeWorker(factory_t *pFactory);
// 从任务队列中得到socket文件，与客户端交互
int handleEvent(ClientInfo *ci,factory_t *pFactory);
// 线程函数指针传入的执行函数
void* threadFunc(void *pArgs);
// 如果使用了cancel线程，则使用cleanFunc解锁退出线程，否则解锁
void cleanFunc(void *pArgs);
#endif
//任务队列的设计
//taskQueue.h