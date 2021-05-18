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

#define SFS_PNODE_PRES_BIT      0b10000000
#define SFS_PNODE_USE_BIT       0b01000000
#define SFS_PNODE_INODE_BIT     0b00100000
#define SFS_PNODE_LINK_BIT      0b00010000
#define SFS_PNODE_COUNT_BITS    0b00001111

#define SFS_FNODE_USE_BIT       0b10000000
#define SFS_FNODE_LOCK_BIT      0b01000000
#define SFS_FNODE_OPEN_BIT      0b00100000
#define SFS_FNODE_WRITE_BIT     0b00010000
#define SFS_FNODE_USR1_BIT      0b00001000
#define SFS_FNODE_USR2_BIT      0b00000100
#define SFS_FNODE_USR3_BIT      0b00000010
#define SFS_FNODE_USR4_BIT      0b00000001

#define SFS_PAGESPACE_CONT      0x03FB
#define SFS_PAGESPACE           0x03FF

#define SFS_PAGE    struct _page   
//#define SFS_FILE    struct _file
#define SFS_FS      struct _fs

#define SFS_WRITE           0x01
#define SFS_APPEND          0x02
#define SFS_START           0x00
#define SFS_CURR            0x01
#define SFS_EOF             0x02

#define SFS_SUCESS          0x00
#define SFS_FNF             0x01
#define SFS_MEM_FULL        0x02

/*
struct _file
{
    int namelen;
    int size;
    char *filename;
    char fnode;
};
*/
struct _page
{
    unsigned char pnode;
    char *data;
};
struct _fs
{
    struct _page *memory;
    int freepages;
    int pagecount;
    int max_filecount;
};
typedef struct _sfs_fd
{
    int file;
    int fptr;
    int size;
} sfs_fd;

/**
 * @brief initiate the server filesystem according to thespecifications
 * @param avaiableMemory    server's memory capacity in MB
 * @param maxFileCount      server's file capacity
 * @return 0 on success, > 0 otherwise
 */
int init_server_fs(SFS_FS *fs, int avaiableMemory, int maxFileCount);

sfs_fd* sfs_create(SFS_FS *fs, char *path, void *buff, size_t size);

sfs_fd* sfs_open(SFS_FS *fs, char *path);

int sfs_close(SFS_FS *fs, sfs_fd *file);

int sfs_read(SFS_FS *fs, sfs_fd *file, void *buff, size_t size);

int sfs_write(SFS_FS *fs, sfs_fd *file, void *buff, size_t size);

int sfs_evictpages(SFS_FS *fs, int npages);

int sfs_pages_required_byfile(char *path, size_t size);

#endif
