#ifndef SERVER_FS
#define SERVER_FS

#define SFS_FS sfs_fs

#define SFS_FD_LOCK     128 //0b10000000
#define SFS_FD_REQSTS   112 //0b01110000
#define SFS_FD_CLIENTS  15  //0b00001111

#define SFS_FILENOTFOUND    1
#define SFS_FILEINSTORAGE   2
#define SFS_FILELOCKED      3
#define SFS_MEMORYFULL      4
#define SFS_WRONGCALL       5


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
    char *data;
    size_t datalen;
    unsigned char finfo;
    int clientlock;
    mutex_t lock;
} sfs_file;
typedef struct _sfs_fd
{
    sfs_file *file;
    int i;
} sfs_fd;
typedef struct _sfs_fs
{
    int currentSize;
    int maxSize;
    int currentFiles;
    int maxFiles;
    mutex_t lock;
    sfs_file **filetable;
} sfs_fs;

int sfs_init(sfs_fs *server_fs, int maxMB, int maxFiles);

retptr sfs_find(sfs_fs *server_fs, const char *name);

retptr sfs_open(sfs_fs *server_fs, const char *name, const char mode);

int sfs_close(sfs_fs *server_fs, sfs_fd *file);

int sfs_write(sfs_fs *server_fs, sfs_fd *file, const void *data, const size_t size);

int sfs_read(sfs_fd *file, void *data, const size_t size);

int sfs_append(sfs_fs *server_fs, sfs_fd *file, const void *data, const size_t size);

int sfs_lock(sfs_fd *file, int client);

int sfs_unlock(sfs_fd *file, int client);

int sfs_islocked(sfs_fd *file, int client);

retptr sfs_create(sfs_fs *server_fs, const char *name, const void *data, const size_t size);

int sfs_remove(sfs_fs *server_fs, const char *name);

int sfs_evict(sfs_fs *server_fs, size_t size);

int sfs_free_file(sfs_file **file);

sfs_file *sfs_alloc_file(const char *name, const void *data, const size_t size);

int sfs_insert(sfs_fs *server_fs, sfs_file *file);

int sfs_filetable_hash(const char *name, int nslots);

int sfs_foreach(sfs_fs *server_fs, void (*proc)(void *));

int sfs_evictcompare(void *a, void *b);

int sfs_printfiletable(sfs_fs *server_fs);

void sfs_printfile(void *f);

#endif