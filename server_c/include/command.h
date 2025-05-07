#include "func.h"
#include"factory.h"
#ifndef __COMMAND__
#define __COMMAND__


int cmd_cd(ClientInfo *ci, const char *path);
int cmd_ls(ClientInfo *ci);
int cmd_pwd(ClientInfo *ci);
int cmd_puts(ClientInfo *ci);
int cmd_gets(ClientInfo *ci, const char *filename);
int cmd_remove(ClientInfo *ci, const char *filename);
int dispatchCommand( ClientInfo *ci, const char *cmdline);
void build_tree(const char *path, int depth, char *buffer, size_t *offset, size_t bufsize);
int cmd_tree(ClientInfo *ci, const char* path);


#endif