#ifndef SERVER_FS
#define SERVER_FS

#pragma once
#include <stdlib.h>
#include <string.h>

#include <error_utils.h>
#include <common.h>
#include <commonio.h>
#include <lock_utils.h>
#include <cond_utils.h>

typedef struct _sfs_folder
{

} sfs_folder;
typedef struct _sfs_file
{

} sfs_file;
typedef struct _sfs_fd
{

} sfs_fd;
typedef struct _sfs_fs
{
    int currentMB;
    int maxMB;
    int currentFiles;
    int maxFiles;
    mutex_t lock;

} sfs_fs;

#endif