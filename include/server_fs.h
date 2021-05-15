#ifndef _SERVER_FS_H
#define _SERVER_FS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lock_utils.h"
#include "cond_utils.h"
#include "common.h"
#include "error_utils.h"

#define SFS_LOCK        0x00000001
#define SFS_OPEN        0x00000002
#define SFS_READ        0x00000004
#define SFS_WRITE       0x00000008
#define SFS_FILE        0x00000010
#define SFS_DIR         0x00000020

#define SFS_INODE_FLAG  0x000000FF
#define SFS_INODE_PTR   0xFFFFFF00

#define SFS_WRITE       0x01
#define SFS_APPEND      0x02
#define SFS_START       0x00
#define SFS_CURR        0x01
#define SFS_EOF         0x02

#define SFS_SUCESS      0x00
#define SFS_FNF         0x01
#define SFS_MEM_FULL    0x02

#define SFS_BLOCKSIZE   0x0400


typedef struct _server_fs_node
{
    char *filename;
    struct _server_fs_node *prev;
    struct _server_fs_node *next;
    struct _server_fs_node *sub;
    void *data;
    int size;
    unsigned int inode;
} server_fs_node;

struct _inode_ptr
{
    char ptr[3];
};

typedef struct _server_memory
{
    int currentFileCapacity;
    int currentMemoryCapacity;
    server_fs_node *root; 
    short blocksize;
    struct _inode_ptr* filtable;
    void *data;
} server_fs;


server_fs init_server_fs(int maxCapacity, int maxFileCount);
int sfs_create(server_fs *fs, char *path, int size, void *buff);
int sfs_delete(char *path);
int sfs_write(void *buff, int len);
int sfs_read(void *buff, int len);
int sfs_open(char *path, int mode);
int sfs_close();
int sfs_lock();
int sfs_unlock();
int sfs_seek(int from, int offs);
int sfs_evict(int mem_to_free);

#endif
