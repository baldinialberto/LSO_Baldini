#ifndef SERVER_FS
#define SERVER_FS

#define SFS_FS sfs_fs

#define SFS_FD_LOCK     0b10000000
#define SFS_FD_REQSTS   0b01110000
#define SFS_FD_CLIENTS  0b00001111

#pragma once
#include <stdlib.h>
#include <string.h>

#include <error_utils.h>
#include <common.h>
#include <commonio.h>
#include <lock_utils.h>
#include <cond_utils.h>

typedef struct _sfs_file
{
    struct _sfs_file *next;
    char *name;
    void *data;
    size_t datalen;
    unsigned char finfo;
    mutex_t lock;
} sfs_file;
typedef struct _sfs_folder
{
    struct _sfs_folder *prev;
    struct _sfs_folder *next;
    struct _sfs_folder *sub;
    sfs_file *files;
    char *name;
} sfs_folder;
typedef struct _sfs_fd
{
    sfs_file *file;
    int i;
} sfs_fd;
typedef struct _sfs_fs
{
    int currentMB;
    int maxMB;
    int currentFiles;
    int maxFiles;
    mutex_t lock;
    sfs_folder *root;
} sfs_fs;

int sfs_init(sfs_fs *server_fs, int maxMB, int maxFiles);

sfs_file *sfs_find(sfs_fs *server_fs, const char *name);

retptr sfs_open(sfs_fs *server_fs, const char *name, const char mode);

#endif