//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_FILE_UTILS_H
#define LSO_LIB_FILE_UTILS_H

#pragma once
#include <stdlib.h>
#include <pthread.h>
#include "hash_utils.h"
#include "list_utils.h"
#include "thread_utils.h"

typedef struct utils_file_data {
	void* data;
	size_t data_len;
	int client;
	unsigned int data_info;
	mutex_t mutex;
} u_file_data;
typedef struct utils_file {
	const char* path;
	u_file_data* file_data;
} u_file;

typedef struct utils_file_storage {
	u_hash_table table;
	size_t max_files;
	size_t curr_files;
	size_t max_bytes;
	size_t curr_bytes;
	mutex_t mutex;
} u_file_storage;

size_t fu_storage_avBytes(u_file_storage* file_storage);
size_t fu_storage_avFiles(u_file_storage* file_storage);
u_file_storage fu_init_file_storage(size_t max_files, size_t max_bytes);
u_file_data* fu_alloc_file_data(void* data, size_t data_len);
size_t fu_storage_files_available(u_file_storage* file_storage);
size_t fu_storage_bytes_available(u_file_storage* file_storage);
void fu_storage_print(u_file_storage* file_storage);
int fu_add_file(u_file_storage* file_storage, u_file_data* file, const char* filepath);
int fu_remove_file(u_file_storage* file_storage, const char* path);
u_file_data* fu_getfile(u_file_storage* file_storage, const char* path);
void fu_storage_free(u_file_storage* file_storage);
int fu_writefile(u_file_data* file, size_t i, void* data, size_t data_len);
size_t fu_nreadfile(u_file_data* file, size_t i, void* dest, size_t dest_len);
size_t fu_readfile(u_file_data* file, size_t i, void** dest);
int fu_file_read(const char* path, u_file_data* dest_file);
int fu_file_write(u_file_data* file, const char* dest_path);
void fu_free_file_data(void* file);
int fu_read_from_path(const char* path, void** data, size_t* data_len);
int fu_write_to_path(const char* path, void* data, size_t data_len);
void fu_print_file(const void* file);
int fu_evict_compare(const void *a, const void *b);
u_list fu_evict_files(u_file_storage* file_storage, size_t bytes_to_free);
void fu_ensure_path(const char* path);

#endif //LSO_LIB_FILE_UTILS_H
