#include"../include/factory.h"
#include"../include/transfer.h"
int factoryInit(factory_t *pFactory, int threadNum){
    bzero(pFactory,sizeof(factory_t));
    pFactory->runningFlag=1;
    pFactory->threadNum = threadNum;
    pFactory->tidArr = (pthread_t *)calloc(threadNum,sizeof(pthread_t));
    pthread_cond_init(&pFactory->taskQueue.cond,NULL);
    bzero(&pFactory->taskQueue,sizeof(taskQueue_t));
    pthread_mutex_init(&pFactory->taskQueue.mutex,NULL);
}
void makeWorker(factory_t *pFactory){
    for(int i = 0;i < pFactory->threadNum; ++i){
        pthread_create(pFactory->tidArr+i,NULL,threadFunc,(void*)pFactory);
    }
}    
void cleanFunc(void *pArgs){
    factory_t *pFactory = (factory_t *)pArgs;
    pthread_mutex_unlock(&pFactory->taskQueue.mutex);
}
    
void* threadFunc(void *pArgs)
{
    int netFd;
    while(1){
        factory_t *pFactory = (factory_t *)pArgs;
        ClientInfo ci;
        pthread_mutex_lock(&pFactory->taskQueue.mutex);
        pthread_cleanup_push(cleanFunc,(void *)pFactory);
        while(pFactory->taskQueue.queueSize == 0){
            pthread_cond_wait(&pFactory->taskQueue.cond,&pFactory->taskQueue.mutex);
            if(pFactory->runningFlag == 0){
                puts("child exit");
                pthread_exit(NULL);
            }       
        }
        printf("Get Task!\n");
        
        ci.clientFd=pFactory->taskQueue.pFront->netFd;
        // 拿当前服务器真实路径
        if (getcwd(ci.currentPath, sizeof(ci.currentPath)) == NULL) {
            perror("getcwd");
            // 出错处理，比如return -1;
        }
        taskDeQueue(&pFactory->taskQueue);
        
        pthread_cleanup_pop(1);
        handleEvent(&ci,pFactory);
        
        printf("pthread done! tid = %lu\n", pthread_self());
    }
}
int handleEvent(ClientInfo *ci,factory_t *pFactory){
    // recvFile(netFd);

    while (1) {
        
        char buf[1024] = {0};
        int len = recv(ci->clientFd, buf, sizeof(buf), 0);
        if (len <= 0) {
            
            break;
        }
        dispatchCommand(ci, buf);  // 分发处理
    
        
        if(pFactory->runningFlag == 0){
            puts("child exit");
            
            pthread_exit(NULL);
        }
    }
    return 0;
}

    