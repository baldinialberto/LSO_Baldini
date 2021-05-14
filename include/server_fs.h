#ifndef _SERVER_FS_H
#define _SERVER_FS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lock_utils.h"
#include "cond_utils.h"
#include "common.h"
#include "error_utils.h"

#define SFS_LOCK    0x01
#define SFS_OPEN    0x02
#define SFS_READ    0x04
#define SFS_WRITE   0x08
#define SFS_APPEND  0x10
#define SFS_CURR    0x20
#define SFS_START   0x40
#define SFS_EOF     0x80

#define SFS_FNF     -0x01

typedef int sfs_fileinode;
typedef struct _sfs_file
{
    char *filename;
    void *data;
    int size;
    int pos;
    sfs_fileinode *flags;
} sfs_file;

sfs_file* sfs_create(char *path, int size, void *buff);
int sfs_delete(char *path);
int sfs_write(sfs_file *file_descriptor, void *buff, int len);
int sfs_read(sfs_file *file_descriptor, void *buff, int len);
sfs_file *sfs_open(char *path, int mode);
int sfs_close(sfs_file *file_descriptor);
int sfs_lock(sfs_file *file_descriptor);
int sfs_unlock(sfs_file *file_descriptor);
int sfs_seek(sfs_file *file_descriptor, int from, int offs);


#endif
