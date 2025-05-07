#include"../include/factory.h"
#include"../include/server.h"
int exitPipe[2];
void sigFunc(int signum){
    printf("signum = %d\n", signum);
    write(exitPipe[1],"1",1);
    puts("Parent exit!");
}

int main(int argc, char *argv[]){
    //./main 192.168.135.132 5678 10
    ARGS_CHECK(argc,4);
    pipe(exitPipe);
    if(fork() != 0){
        close(exitPipe[0]);
        signal(SIGUSR1,sigFunc);
        wait(NULL);
        exit(0);
    }
    close(exitPipe[1]);
    
    int workerNum = atoi(argv[3]);
    factory_t factory;
    factoryInit(&factory,workerNum);
    makeWorker(&factory);
    int sockFd;
    tcpInit(argv[1],argv[2], &sockFd);
    int epfd = epollCtor();
    
    
    epollAdd(sockFd,epfd);
    struct epoll_event evs[2];
    while(1){
        int readyNum = epoll_wait(epfd,evs,2,-1);
        for(int i = 0;i < readyNum; ++i){
            if(evs[i].data.fd == sockFd){
                int netFd = accept(sockFd, NULL, NULL);
                pthread_mutex_lock(&factory.taskQueue.mutex);
                taskEnQueue(&factory.taskQueue, netFd);
                printf("New Task!\n");
                pthread_cond_signal(&factory.taskQueue.cond);
                pthread_mutex_unlock(&factory.taskQueue.mutex);
            }
            else if(evs[i].data.fd == exitPipe[0]){
                puts("exit threadPool!");
                factory.runningFlag = 0;
                pthread_cond_broadcast(&factory.taskQueue.cond);
                for(int j = 0; j < workerNum; ++j){
                    pthread_join(factory.tidArr[j],NULL);
                }
                puts("done");
                exit(0);
                }
                
        }
    }
}