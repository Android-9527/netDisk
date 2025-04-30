#include"func.h"
#include"factory.h"
#ifndef __TRANSFER__
#define __TRANSFER__
typedef struct train_s{
    int dataLength;
    char buf[1000];
} train_t;
int sendn(int netFd,const void* pstart,int len);
int recvn(int netFd,const void* pstart,int len);
int recvCmd(int netFd);  //接受train，print train.buf
// int sendCmd(int sockfd,const char *pstart, int len);
int sendTrain(int newFw,const void* pstart);
int recvFile(ClientInfo *ci);
int sendFile(ClientInfo *ci,const char* path);
#endif