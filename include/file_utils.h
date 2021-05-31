#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "hash_utils.h"
#include "string_utils.h"
#include "mem_utils.h"

typedef struct _utils_file_data
{
    void *data;
    size_t datalen;
    int client;
    unsigned int datainfo;
    pthread_mutex_t mutex;
} u_file_data;
typedef struct _utils_file
{
    char *path;
    u_file_data *data;
} u_file;

typedef struct _utils_file_storage
{
    u_hash_table table;
    size_t maxfiles;
    size_t nfiles;
    size_t maxBytes;
    size_t nBytes;
} u_file_storage;

size_t fu_storage_avBytes(u_file_storage *fstorage);
size_t fu_storage_avFiles(u_file_storage *fstorage);
u_file_storage fu_init_file_storage(size_t maxfiles, size_t maxBytes);
u_file fu_allocfile(char *path, u_file_data *data);
u_file fu_allocfile_empty(char *path);
u_file fu_allocfile_oncopy(char *path, const void *data, size_t datalen);
size_t fu_storage_filesavailable(u_file_storage *fstorage);
size_t fu_storage_bytesavailable(u_file_storage *fstorage);
int fu_addfile(u_file_storage *fstorage, u_file_data *file, char *filepath);
int fu_removefile(u_file_storage *fstorage, char *path);
u_file_data *fu_getfile(u_file_storage *fstorage, char *path);
void fu_storage_free(u_file_storage *fstorage);
int fu_writefile(u_file_data *file, size_t i, void *data, size_t datalen);
size_t fu_readfile(u_file_data *file, size_t i, void *dest, size_t destlen);
size_t fu_readfile_realloc(u_file_data *file, size_t i, void **dest);
int fu_fread(const char *path, u_file_data *destfile);
int fu_fwrite(u_file_data *file, const char *destpath);
size_t fu_filehash(void *file);
int fu_filenamecompare(void *fileA, void *fileB);
void *fu_filepath(void *file);
void *fu_filedata(void *file);
void fu_fileprint(void *file);
void fu_filedatafree(void *file);
int fu_readpath(const char *path, void **data, size_t *datalen);
int fu_writepath(const char *path, void *data, size_t datalen);

#endif