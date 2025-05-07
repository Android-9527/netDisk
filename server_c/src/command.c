#include"../include/command.h"
#include"../include/transfer.h"
int dispatchCommand(ClientInfo *ci, const char *cmdline) {
    char cmd[64] = {0};
    char arg[256] = {0};
    // printf("%s\n",cmdline);
    // 拆分指令与参数（类似 sscanf）
    sscanf(cmdline, "%s %s", cmd, arg);

    if (strcmp(cmd, "cd") == 0) {
        return cmd_cd(ci, arg);
    } else if (strcmp(cmd, "ls") == 0) {
        return cmd_ls(ci);
    } else if (strcmp(cmd, "puts") == 0) {
        return cmd_puts(ci);
    } else if (strcmp(cmd, "gets") == 0) {
        return cmd_gets(ci, arg);
    } else if (strcmp(cmd, "pwd") == 0) {
        return cmd_pwd(ci);
    } else if (strcmp(cmd, "remove") == 0) {
        return cmd_remove(ci, arg);
    } else if(strcmp(cmd, "tree") == 0) {
        return cmd_tree(ci, arg);
    }else{
        sendTrain(ci->clientFd, "ERROR: Unknown command");
        
    }
}


int cmd_ls(ClientInfo *ci){
    DIR *dir;
    struct dirent *entry;
    char result[1000] = {0}; // 保存所有文件名
    size_t used = 0;

    dir = opendir(ci->currentPath);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        size_t name_len = strlen(entry->d_name);
        if (used + name_len + 2 >= sizeof(result)) {
            fprintf(stderr, "结果字符串太长！\n");
            break;
        }

        strcat(result, entry->d_name);
        strcat(result, " "); // 添加空格分隔
        used += name_len + 1;
    }

    closedir(dir);

    sendTrain(ci->clientFd,result);

    
    

    // printf("当前目录的文件和目录:\n%s\n", result);
    return 0;
}

int cmd_pwd(ClientInfo *ci){
    sendTrain(ci->clientFd,ci->currentPath); 
    return 0;
}


int cmd_cd(ClientInfo *ci, const char* path) {
    
    char newPath[1024];
    snprintf(newPath, sizeof(newPath), "%s/%s", ci->currentPath, path);

    struct stat st;
    if (stat(newPath, &st) == -1 || !S_ISDIR(st.st_mode)) {
        const char *msg = "cd failed";
        sendTrain(ci->clientFd, msg);
        perror("stat");
        return -1;
    }

    // 更新client自己的路径
    strcpy(ci->currentPath, newPath);

    const char *msg = "cd success";
    sendTrain(ci->clientFd, msg);
    return 0;
}

int remove_dir(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return -1;
    }
    struct dirent *entry;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(filepath, &st) == -1) {
            perror("stat");
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(st.st_mode)) {
            // 是子目录，递归删除
            if (remove_dir(filepath) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            // 是普通文件，直接删除
            if (unlink(filepath) == -1) {
                perror("unlink");
                closedir(dir);
                return -1;
            }
        }
    }
    closedir(dir);
    // 删除空目录
    if (rmdir(path) == -1) {
        perror("rmdir");
        return -1;
    }
    return 0;
}

int cmd_remove(ClientInfo *ci, const char *filename) {
    struct stat st;
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", ci->currentPath, filename);

    int ret = stat(fullPath, &st);
    if (ret == -1) {
        // 文件不存在
        const char *msg = "remove failed: file not found";
        
        sendTrain(ci->clientFd, msg);
        perror("stat");
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        // 是目录
        ret = remove_dir(fullPath);
    } else {
        // 是普通文件
        ret = unlink(fullPath);
        if (ret == -1) {
            perror("unlink");
        }
    }

    if (ret == -1) {
        const char *msg = "remove failed";
        sendTrain(ci->clientFd, msg);
    } else {
        const char *msg = "remove success";
        sendTrain(ci->clientFd, msg);
    }
    return ret;
}

void build_tree(const char *path, int depth, char *buffer, size_t *offset, size_t bufsize) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char filepath[1024];
    struct stat st;

    while ((entry = readdir(dir)) != NULL) {
        // 跳过.和..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构造完整路径
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        if (stat(filepath, &st) == -1) {
            perror("stat");
            continue;
        }

        // 构建一行，增加缩进，每层4空格
        int n = snprintf(buffer + *offset, bufsize - *offset, "%*s%s\n", depth * 4, "", entry->d_name);
        *offset += n; // 更新偏移量

        // 如果是目录，递归
        if (S_ISDIR(st.st_mode)) {
            build_tree(filepath, depth + 1, buffer, offset, bufsize);
        }
    }

    closedir(dir);
}

// 外部接口：调用这个函数
int cmd_tree(ClientInfo *ci, const char *path) {
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", ci->currentPath, path);

    char *buffer = (char *)malloc(1024*2);
    if (!buffer) {
        perror("malloc");
        return -1;
    }
    size_t offset = 0;
    memset(buffer, 0, 1024*2);

    build_tree(fullPath, 0, buffer, &offset, 1024*2);

    
    sendTrain(ci->clientFd,buffer);
    
    
    free(buffer);
    

    
    return 0;
}

int cmd_puts(ClientInfo *ci){
    
    recvFile(ci);
    return 0;
}
int cmd_gets(ClientInfo *ci, const char *filename){
    
    sendFile(ci,filename);
    return 0;
}