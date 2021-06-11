//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_FILE_UTILS_H
#define LSO_LIB_FILE_UTILS_H

#pragma once
#include <stdlib.h>
#include <pthread.h>
#include "hash_utils.h"

typedef struct utils_file_data
{
    void *data;
    size_t data_len;
    int client;
    unsigned int data_info;
    pthread_mutex_t mutex;
} u_file_data;
typedef struct utils_file
{
    char *path;
    u_file_data *data;
} u_file;

typedef struct utils_file_storage
{
    u_hash_table table;
    size_t max_files;
    size_t n_files;
    size_t max_bytes;
    size_t n_bytes;
} u_file_storage;

size_t fu_storage_avBytes(u_file_storage *file_storage);
size_t fu_storage_avFiles(u_file_storage *file_storage);
u_file_storage fu_init_file_storage(size_t max_files, size_t max_bytes);
u_file fu_allocfile(char *path, u_file_data *data);
u_file fu_allocfile_empty(char *path);
u_file fu_allocfile_oncopy(char *path, const void *data, size_t data_len);
size_t fu_storage_files_available(u_file_storage *file_storage);
size_t fu_storage_bytes_available(u_file_storage *file_storage);
void fu_storage_print(u_file_storage *file_storage);
int fu_add_file(u_file_storage *file_storage, u_file_data *file, char *filepath);
int fu_remove_file(u_file_storage *file_storage, char *path);
u_file_data *fu_getfile(u_file_storage *file_storage, char *path);
void fu_storage_free(u_file_storage *file_storage);
int fu_writefile(u_file_data *file, size_t i, void *data, size_t data_len);
size_t fu_readfile(u_file_data *file, size_t i, void *dest, size_t dest_len);
size_t fu_readfile_realloc(u_file_data *file, size_t i, void **dest);
int fu_fd_read(const char *path, u_file_data *dest_file);
int fu_fd_write(u_file_data *file, const char *dest_path);
size_t fu_filehash(void *file);
int fu_filenamecompare(void *fileA, void *fileB);
void *fu_filepath(void *file);
void *fu_filedata(void *file);
void fu_fileprint(void *file);
void fu_fileprint_hash(void *file);
void fu_filedatafree(void *file);
int fu_readpath(const char *path, void **data, size_t *data_len);
int fu_writepath(const char *path, void *data, size_t data_len);


#endif //LSO_LIB_FILE_UTILS_H
