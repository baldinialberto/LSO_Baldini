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

#define SFS_INODE_FC        0xFF000000
// inode mask defines
#define SFS_INODE_F1        0x00FF0000
#define SFS_INODE_F2        0x0000FF00
#define SFS_INODE_F3        0x000000FF
#define SFS_INODE_F1RS      0x00000008
#define SFS_INODE_F2RS      0x00000010
#define SFS_INODE_F3RS      0x00000018

#define SFS_INODE_FLOCK     0b10000000
#define SFS_INODE_FOPEN     0b01000000
#define SFS_INODE_FWRITE    0b00100000
#define SFS_INODE_FDIR      0b00010000
#define SFS_INODE_ACCESS    0b00001111

#define SFS_FNODE_SIZE      0x0154
#define SFS_FNODE_CHARS     0x0148

#define SFS_PAGE_SIZE       0x0400
#define SFS_PAGED_SIZE      0x03F8

#define SFS_PAGE    struct _page   
#define SFS_FILE    struct _file
#define SFS_FS      struct _fs

#define SFS_WRITE           0x01
#define SFS_APPEND          0x02
#define SFS_START           0x00
#define SFS_CURR            0x01
#define SFS_EOF             0x02

#define SFS_SUCESS          0x00
#define SFS_FNF             0x01
#define SFS_MEM_FULL        0x02

struct _file
{
    char filename[SFS_FNODE_CHARS];
    int page;
    int size;
};
struct _page
{
    int inode;
    char *data;
    int next;
};
struct _fs
{
    struct _page *memory;
    int pagecount;
    int max_filecount;
};

/**
 * @brief initiate the server filesystem according to thespecifications
 * @param avaiableMemory    server's memory capacity in MB
 * @param maxFileCount      server's file capacity
 * @return 0 on success, > 0 otherwise
 */
int init_server_fs(SFS_FS *fs, int avaiableMemory, int maxFileCount);



#endif
