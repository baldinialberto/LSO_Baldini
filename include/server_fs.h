#ifndef _SERVER_FS_H
#define _SERVER_FS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "locks"
#include "condvars"
#include "common.h"
#include "error_utils.h"

typedef struct _sfs_file
{
    char *filename;
    void *data;
    size_t size;
    size_t pos;
    lock_t lock;
}

int sfs_write(sfs_fd file_descriptor, void *buff, size_t )

#endif
