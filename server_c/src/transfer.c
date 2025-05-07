#include"../include/transfer.h"

int recvn(int sockfd,const void *pstart, int len) {
    int total = 0;
    int ret;
    char *p = (char *)pstart;
    while (total < len) {
        ret = recv(sockfd, p + total, len - total, 0);
        if (ret <= 0) return -1;
        total += ret;
    }
    return 0;
}

int sendn(int sockfd,const void *pstart, int len) {
    int total = 0;
    int ret;
    char *p = (char *)pstart;
    while (total < len) {
        ret = send(sockfd, p + total, len - total, 0);
        if (ret <= 0) return -1; // 断开或错误
        total += ret;
    }
    return 0;
}
int sendCmd(int sockfd,const char *pstart, int len){
    train_t t;
    bzero(&t,sizeof(t));
    //先接收结果长度
    if (sendn(sockfd, &t.dataLength, sizeof(int)) == -1) {
        printf("recvCmd: 对方断开或接收失败\n");
        return -1;
    }
    // printf("%d\n",t.dataLength);
    //再接收结果
    
    if (recvn(sockfd, t.buf, t.dataLength) == -1) {
        printf("recvCmd: 对方断开或接收失败\n");
        return -1;
    }
    printf("%s\n",t.buf);    
}
int recvCmd(int netFd)
{
    train_t t;
    bzero(&t,sizeof(t));
    //先接收结果长度
    if (recvn(netFd, &t.dataLength, sizeof(int)) == -1) {
        printf("recvCmd: 对方断开或接收失败\n");
        return -1;
    }
    // printf("%d\n",t.dataLength);
    //再接收结果
    
    if (recvn(netFd, t.buf, t.dataLength) == -1) {
        printf("recvCmd: 对方断开或接收失败\n");
        return -1;
    }
    printf("%s\n",t.buf);
    
}
int recvFile(ClientInfo *ci)
{   
    train_t t;
    bzero(&t,sizeof(t));
    //先接收文件名长度
    if (recvn(ci->clientFd, &t.dataLength, sizeof(int)) == -1) {
        printf("recvFile: 对方断开或接收失败\n");
        return -1;
    }
    
    //再接收文件名
    
    if (recvn(ci->clientFd, t.buf, t.dataLength) == -1) {
        printf("recvFile: 对方断开或接收失败\n");
        return -1;
    }
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", ci->currentPath, t.buf);

    //接收方创建一个同名文件
    int fd = open(fullPath, O_WRONLY | O_CREAT, 0666);
    printf("%s----\n",fullPath);
    ERROR_CHECK(fd, -1, "open");
    
    while (1){
        bzero(&t,sizeof(t));
        recvn(ci->clientFd,&t.dataLength,sizeof(int));
        if(0 == t.dataLength){
            break;
        }
        if (recvn(ci->clientFd, t.buf, t.dataLength) == -1) {
            printf("recvFile: 对方断开或接收失败\n");
            return -1;
        }
        write(fd,t.buf,t.dataLength);
    }
    close(fd);
    
}
int sendFile(ClientInfo *ci,const char* path){

    sendTrain(ci->clientFd,path);
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", ci->currentPath, path);
    int fd = open(fullPath,O_RDONLY);
    ERROR_CHECK(fd,-1,"open");
    train_t t;
    bzero(&t,sizeof(t));
    while(1){
        t.dataLength = read(fd,t.buf,sizeof(t.buf));
        ERROR_CHECK(t.dataLength,-1,"read");
        if(t.dataLength != sizeof(t.buf)){
            printf("t.dataLength = %d\n",t.dataLength);
        }
        if(t.dataLength == 0){
            bzero(&t,sizeof(t));
            send(ci->clientFd,&t,4,MSG_NOSIGNAL);
            break;
        }
        int ret =
        send(ci->clientFd,&t,sizeof(int)+t.dataLength,MSG_NOSIGNAL);
        if(ret == -1){
            perror("send");
            break;
        }
    }
    close(fd);
    return 0;
}
int sendTrain(int newFw,const void* pstart){
    train_t train;
    char *result=(char *)pstart;
    memset(&train, 0, sizeof(train));
    strncpy(train.buf, result, sizeof(train.buf));
    train.dataLength = strlen(train.buf); // 有效长度
    // 发送前 4 字节长度 + 有效内容
    sendn(newFw, &train, 4 + train.dataLength);
}