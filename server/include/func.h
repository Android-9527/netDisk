#ifndef __WD_FUNC_H
#define __WD_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/socket.h>   // 关键：包含 socket 通信结构体定义
#include <sys/uio.h>      // 包含 struct iovec, readv, writev
#include <netinet/in.h>   // sockaddr_in 等定义
#include <arpa/inet.h>    // inet_pton 等函数
#include <errno.h>
#include <sys/epoll.h>
#include <bits/pthreadtypes.h>
#include <signal.h>
#include <pthread.h>

#define ARGS_CHECK(argc, n) { \
    if (argc != n) { \
        fprintf(stderr, "ERROR: expected %d arguments.\n", n); \
        exit(1); \
    } \
}

#define ERROR_CHECK(retval, val, msg) { \
    if (retval == val) { \
        perror(msg); \
        exit(1); \
    } \
}

#define SIZE(a) (sizeof(a)/sizeof(a[0]))



#endif
