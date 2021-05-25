#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_utils.h"
#include "string_utils.h"
#include "mem_utils.h"

typedef struct _utils_file
{
    u_string path;
    void *data;
    size_t datalen;
} u_file;
typedef struct _utils_file_storage
{
    u_hash_table table;
    size_t maxfiles;
    size_t nfiles;
    size_t maxBytes;
    size_t nBytes;
} u_file_storage;

u_file_storage fu_init_file_storage(size_t maxfiles, size_t maxBytes);
u_file* fu_allocfile(const char *path, const void *data, size_t datalen);
u_file* fu_allocfile_empty(const char *path);
u_file* fu_allocfile_oncopy(const char *path, const void *data, size_t datalen);
void fu_freefile(u_file *file);
int fu_writefile(u_file *file, size_t i, void *data, size_t datalen);
size_t fu_readfile(u_file *file, size_t i, void *dest, size_t destlen);
size_t fu_readfile_realloc(u_file *file, size_t i, void **dest);
size_t fu_filehash(void *file);
int fu_filecompare(void *fileA, void *fileB);
void *fu_filepath(void *file);
void *fu_filedata(void *file);
void fu_fileprint(void *file);
void fu_filefree(void *file);

#endif