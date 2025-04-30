#include "../server/include/func.h"
#include "../server/include/transfer.h"
#include "../server/include/factory.h"


int main(int argc, char *argv[])
{
    ARGS_CHECK(argc, 3);
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));
    int ret = connect(sockFd, (struct sockaddr *)&addr,
    sizeof(struct sockaddr_in));
    ERROR_CHECK(ret, -1, "connect");
    ClientInfo ci;
    ci.clientFd=sockFd;
    // 拿当前客户端真实路径
    if (getcwd(ci.currentPath, sizeof(ci.currentPath)) == NULL) {
        perror("getcwd");
        // 出错处理，比如return -1;
    }
    //输入读取命令
    char cmdBuf[1024] = {0};
    while (1) {
        printf("请输入命令（cd/ls/puts/gets/remove/pwd/tree/exit）: ");
        bzero(cmdBuf, sizeof(cmdBuf));
        fgets(cmdBuf, sizeof(cmdBuf), stdin);
        cmdBuf[strcspn(cmdBuf, "\n")] = 0; // 去掉换行符

        // 用户输入 exit 退出客户端
        if (strcmp(cmdBuf, "exit") == 0) {
            printf("客户端退出\n");
            break;
        }

        // 将命令发送给服务端
        ret = send(sockFd, cmdBuf, strlen(cmdBuf), 0);

        if (ret <= 0) {
            perror("send");
            break;
        }
        char cmd[64] = {0};
        char arg[256] = {0};
        // printf("%s\n",cmdline);
        // 拆分指令与参数（类似 sscanf）
        sscanf(cmdBuf, "%s %s", cmd, arg);
        if (strcmp(cmd, "puts") == 0) {
            sendFile(&ci, arg);
        } else if (strcmp(cmd, "gets") == 0) {
            recvFile(&ci);
        }else{
            recvCmd(sockFd);
        }
    }
    // sendFile(sockFd);
    close(sockFd);
    return 0;
}




    
    

