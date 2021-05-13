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

typedef int sfs_fileinode;
typedef struct _sfs_file
{
    char *filename;
    void *data;
    int size;
    int pos;
    sfs_fileinode *flags;
} sfs_file;

int sfs_write(sfs_file file_descriptor, void *buff, int len);
int sfs_read(sfs_file file_descriptor, void *buff, int len);

#endif
