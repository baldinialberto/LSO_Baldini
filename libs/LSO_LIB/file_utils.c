//
// Created by Alberto Baldini on 31/05/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mem_utils.h"
#include "string_utils.h"
#include "array_utils.h"
#include "hash_utils.h"
#include "file_utils.h"
#include "thread_utils.h"

void fu_storage_print(u_file_storage* file_storage)
{
	DEBUG(puts(__func__));
	hu_print(&(file_storage->table));
}

u_file_storage fu_init_file_storage(size_t max_files, size_t max_bytes)
{
	DEBUG(puts(__func__));
	if (max_files == 0)
	{
		fprintf(stdout, "fu_init_file_storage : param max_files == 0\n");
		fflush(stdout);
		return (u_file_storage){ 0 };
	}
	if (max_bytes == 0)
	{
		fprintf(stdout, "fu_init_file_storage : param max_bytes == 0\n");
		fflush(stdout);
		return (u_file_storage){ 0 };
	}
	u_file_storage file_storage;
	file_storage.curr_files = 0;
	file_storage.max_files = max_files;
	file_storage.curr_bytes = 0;
	file_storage.max_bytes = max_bytes;
	mutex_init(file_storage.mutex);
	file_storage.table = hu_init(
		(file_storage.max_files >> 2) + 1,
		hu_hash_string,
		su_string_compare,
		fu_print_file,
		fu_free_file_data,
		mu_free
	);

	return file_storage;
}
u_file_data* fu_alloc_file_data(void* data, size_t data_len)
{
	DEBUG(puts(__func__));
	u_file_data* new_file = mu_malloc(sizeof(u_file_data));
	new_file->data_len = data_len;
	new_file->data = data;
	new_file->data_info = 0;
	new_file->client = -1;
	mutex_init(new_file->mutex);

	return new_file;
}
ssize_t fu_storage_files_available(u_file_storage* file_storage)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL)
	{
		fprintf(stdout, "fu_storage_files_available : wrong params\n");
		fflush(stdout);
		return 0;
	}
	return file_storage->max_files - file_storage->curr_files;
}
ssize_t fu_storage_bytes_available(u_file_storage* file_storage)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL)
	{
		fprintf(stdout, "fu_storage_bytes_available : wrong params\n");
		fflush(stdout);
		return 0;
	}
	return file_storage->max_bytes - file_storage->curr_bytes;
}
int fu_add_file(u_file_storage* file_storage, u_file_data* file, const char* filepath)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL || file == NULL || filepath == NULL)
	{
		fprintf(stdout, "fu_add_file : wrong params\n");
		fflush(stdout);
		return -1;
	}
	if (fu_storage_files_available(file_storage) == 0)
	{
		fprintf(stdout, "fu_add_file : file_storage has stored the maximum amount of files\n");
		fflush(stdout);
		fu_free_file_data(file);
		return -1;
	}
	if (file->data_len > fu_storage_bytes_available(file_storage))
	{
		fprintf(stdout, "fu_add_file : file_storage has not enough space to store this file\n");
		fflush(stdout);
		fu_free_file_data(file);
		return -1;
	}
	file->path = filepath;
	if (hu_insert(&(file_storage->table), filepath, file))
	{
		fprintf(stdout, "fu_add_file : hu_insert has returned an error, file is not stored in file_storage\n");
		fflush(stdout);
		fu_free_file_data(file);
		return -1;
	}
	(file_storage->curr_files)++;
	file_storage->curr_bytes += file->data_len;
	return 0;
}
int fu_remove_file(u_file_storage* file_storage, const char* path)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL || path == NULL)
	{
		fprintf(stdout, "fu_remove_file : wrong params\n");
		fflush(stdout);
		return -1;
	}
	//mutex_lock(file_storage->mutex);
	if (hu_get(&(file_storage->table), path) == NULL)
	{
		fprintf(stdout, "fu_remove_file : path is not stored in storage\n");
		fflush(stdout);
		return -1;
	}
	size_t file_data_len = fu_getfile(file_storage, path)->data_len;
	if (hu_remove(&(file_storage->table), (void*)path))
	{
		fprintf(stdout, "fu_remove_file : hu_remove has returned an error, file is not removed from file_storage\n");
		fflush(stdout);
		return -1;
	}
	(file_storage->curr_files)--;
	file_storage->curr_bytes -= file_data_len;
	return 0;
}
u_file_data* fu_getfile(u_file_storage* file_storage, const char* path)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL || path == NULL)
	{
		fprintf(stdout, "fu_getfile : wrong params\n");
		fflush(stdout);
		return NULL;
	}
	u_file_data* file_data = (u_file_data*)hu_get(&(file_storage->table), (void*)path);
	if (file_data != NULL) file_data->data_info++;
	return file_data;
}
void fu_storage_free(u_file_storage* file_storage)
{
	DEBUG(puts(__func__));
	if (file_storage == NULL)
	{
		fprintf(stdout, "fu_storage_free : param file_storage == NULL\n");
		fflush(stdout);
		return;
	}
	mutex_lock(file_storage->mutex);
	fprintf(stdout, "file storage at exit : %zu files in storage, %zu bytes occupied, serverCapacity = %zu\n",
		file_storage->curr_files, file_storage->curr_bytes, file_storage->max_bytes);
	fprintf(stdout, "file storage at exit : avBytes = %zd, avFiles = %zd\n",
		fu_storage_bytes_available(file_storage), fu_storage_files_available(file_storage));
	hu_free(&(file_storage->table));
	mutex_unlock(file_storage->mutex);
	mutex_destroy(file_storage->mutex);
}
int fu_file_read(const char* path, u_file_data* dest_file)
{
	DEBUG(puts(__func__));
	if (path == NULL)
	{
		fprintf(stdout, "fu_file_read : param path == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (dest_file == NULL)
	{
		fprintf(stdout, "fu_file_read : param dest_file == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (dest_file->data != NULL && dest_file->data_len == 0)
	{
		fprintf(stdout, "fu_file_read : dest_file has file_data in it. Free old file_data\n");
		fflush(stdout);
		free(dest_file->data);
	}
	if (dest_file->data == NULL && dest_file->data_len != 0)
	{
		fprintf(stdout, "fu_file_read : dest_file has wrong data_len in it. Reset data_len\n");
		fflush(stdout);
		dest_file->data_len = 0;
	}
	if (dest_file->data != NULL && dest_file->data_len != 0)
	{
		fprintf(stdout, "fu_file_read : dest_file has file_data in it. Abort\n");
		fflush(stdout);
		return -1;
	}
	FILE* file_to_read = fopen(path, "r");
	if (file_to_read == NULL)
	{
		fprintf(stdout, "fu_file_read : unable to open %s\n", path);
		fflush(stdout);
		return -1;
	}
	size_t i = 0, datalen = 0, datachunk = 1024, readlen;
	char* data = mu_malloc(datachunk);
	while (!feof(file_to_read))
	{
		readlen = fread(data + i, 1, datachunk, file_to_read);
		if (ferror(file_to_read))
		{
			perror("fu_file_read : fread");
			fprintf(stdout, "fu_file_read : unable to read from %s\n", path);
			fflush(stdout);
			if (fclose(file_to_read))
			{
				perror("fu_file_read : fclose");
				fprintf(stdout, "fu_file_read : unable to close %s\n", path);
				fflush(stdout);
			}
			return -1;
		}
		datalen += readlen;
		if (readlen == datachunk)
		{
			i += readlen;
			data = mu_realloc(data, datalen + datachunk);
		}
	}
	if (datalen)
	{
		data = mu_realloc(data, datalen);
	}
	else
	{
		mu_free(data);
		data = NULL;
	}
	dest_file->data = data;
	dest_file->data_len = datalen;
	if (fclose(file_to_read))
	{
		fprintf(stdout, "fu_file_read : unable to close %s\n", path);
		fflush(stdout);
		return -1;
	}
	return 0;
}
int fu_file_write(u_file_data* file, const char* dest_path)
{
	DEBUG(puts(__func__));
	if (file == NULL)
	{
		fprintf(stdout, "fu_file_write : param file == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (file->data == NULL && file->data_len != 0)
	{
		fprintf(stdout, "fu_file_write : param file->file_data == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (file->data_len == 0 && file->data != NULL)
	{
		fprintf(stdout, "fu_file_write : param file->data_len == 0\n");
		fflush(stdout);
		return -1;
	}
	if (dest_path == NULL)
	{
		fprintf(stdout, "fu_file_write : param dest_path == NULL\n");
		fflush(stdout);
		return -1;
	}

	fu_ensure_path(dest_path);
	FILE* file_to_write = fopen(dest_path, "w");

	if (file_to_write == NULL)
	{
		perror("fopen at fu_file_write");
		fprintf(stdout, "fu_file_write : unable to open %s\n", dest_path);
		fflush(stdout);
		return -1;
	}
	fwrite(file->data, 1, file->data_len, file_to_write);
	if (ferror(file_to_write))
	{
		perror("fu_file_write : fwrite");
		fprintf(stdout, "fu_file_write : unable to write in %s\n", dest_path);
		fflush(stdout);
		if (fclose(file_to_write))
		{
			perror("fu_file_write : fclose");
			fprintf(stdout, "fu_file_write : unable to close %s\n", dest_path);
			fflush(stdout);
		}
		return -1;
	}
	if (fclose(file_to_write))
	{
		fprintf(stdout, "fu_file_write : unable to close %s\n", dest_path);
		fflush(stdout);
		return -1;
	}
	return 0;
}
int fu_writefile(u_file_storage* file_storage, u_file_data* file, size_t i, void* data, size_t data_len)
{
	DEBUG(puts(__func__));
	if (file == NULL)
	{
		fprintf(stdout, "fu_writefile : param file == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data == NULL)
	{
		fprintf(stdout, "fu_writefile : param file_data == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data_len == 0)
	{
		fprintf(stdout, "fu_writefile : param data_len == 0\n");
		fflush(stdout);
		return -1;
	}
	if (i > file->data_len)
	{
		fprintf(stdout, "fu_writefile : param i > file::data_len\n");
		fflush(stdout);
		return -1;
	}
	file->data_info++;

	size_t new_size = (i + data_len);
	file_storage->curr_bytes += new_size - file->data_len;

	file->data = mu_realloc(file->data, new_size);
	file->data_len = new_size;

	memcpy((char*)(file->data) + i, data, data_len);

	return 0;
}
size_t fu_nreadfile(u_file_data* file, size_t i, void* dest, size_t dest_len)
{
	DEBUG(puts(__func__));
	if (file == NULL)
	{
		fprintf(stdout, "fu_nreadfile : param file == NULL\n");
		fflush(stdout);
		return 0;
	}
	if (dest == NULL)
	{
		fprintf(stdout, "fu_nreadfile : param dest == NULL\n");
		fflush(stdout);
		return 0;
	}
	if (dest_len == 0)
	{
		fprintf(stdout, "fu_nreadfile : param dest_len == 0\n");
		fflush(stdout);
		return 0;
	}
	if (i >= file->data_len)
	{
		fprintf(stdout, "fu_nreadfile : param i >= file::data_len\n");
		fflush(stdout);
		return 0;
	}
	size_t remainingsize = file->data_len - i;
	if (dest_len > remainingsize) dest_len = remainingsize;
	memcpy(dest, (char*)(file->data) + i, dest_len);

	return dest_len;
}
size_t fu_readfile(u_file_data* file, size_t i, void** dest)
{
	DEBUG(puts(__func__));
	if (file == NULL)
	{
		fprintf(stdout, "fu_readfile : param file == NULL\n");
		fflush(stdout);
		return 0;
	}
	if (dest == NULL)
	{
		fprintf(stdout, "fu_readfile : param dest == NULL\n");
		fflush(stdout);
		return 0;
	}
	if (i >= file->data_len)
	{
		fprintf(stdout, "fu_readfile : param i >= file::data_len\n");
		fflush(stdout);
		return 0;
	}
	file->data_info++;
	size_t remainingsize = file->data_len - i;
	*dest = mu_realloc(*dest, remainingsize);
	return fu_nreadfile(file, i, *dest, remainingsize);
}
void fu_free_file_data(void* file)
{
	DEBUG(puts(__func__));
	if (file == NULL)
	{
		fprintf(stdout, "fu_free_file_data : wrong params\n");
		fflush(stdout);
		return;
	}
	fprintf(stdout, "free file\n");
	fflush(stdout);
	mutex_destroy(((u_file_data*)file)->mutex);
	mu_free(((u_file_data*)file)->data);
	mu_free((u_file_data*)file);
}
int fu_read_from_path(const char* path, void** data, size_t* data_len)
{
	DEBUG(puts(__func__));
	if (path == NULL)
	{
		fprintf(stdout, "fu_read_from_path : param path == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data == NULL)
	{
		fprintf(stdout, "fu_read_from_path : param file_data == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data_len == NULL)
	{
		fprintf(stdout, "fu_read_from_path : param data_len == NULL\n");
		fflush(stdout);
		return -1;
	}
	u_file_data temp = { *data, *data_len, 0 };
	if (fu_file_read(path, &temp) == -1)
	{
		fprintf(stdout, "fu_read_from_path : fu_file_read reaturned an error\n");
		fflush(stdout);
		return -1;
	}
	*data = temp.data;
	*data_len = temp.data_len;
	return 0;
}
int fu_write_to_path(const char* path, void* data, size_t data_len)
{
	DEBUG(puts(__func__));
	if (path == NULL)
	{
		fprintf(stdout, "fu_write_to_path : param path == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data == NULL && data_len != 0)
	{
		fprintf(stdout, "fu_write_to_path : param file_data == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (data != NULL && data_len == 0)
	{
		fprintf(stdout, "fu_write_to_path : param data_len == 0\n");
		fflush(stdout);
		return -1;
	}
	if (data == NULL && data_len == 0)
	{
		fprintf(stdout, "fu_write_to_path : nothing to do\n");
		fflush(stdout);
		return 0;
	}
	u_file_data temp = { data, data_len, 0 };
	return fu_file_write(&temp, path);
}
void fu_print_file(const void* file)
{
	DEBUG(puts(__func__));
	printf("filename = %s\n", ((char*)(((u_hash_item*)file)->key)));
}
int fu_paths_compare(const void* a, const void* b)
{
	DEBUG(puts(__func__));
	return strcmp((*((u_file_data**)a))->path, (*((u_file_data**)b))->path);
}

int fu_evict_compare(const void* a, const void* b)
{
	DEBUG(puts(__func__));
	int compa = (*((u_file_data**)a))->data_info - (*((u_file_data**)b))->data_info;
	ssize_t compb = (*((u_file_data**)a))->data_len - (*((u_file_data**)b))->data_len;
	if (compa == 0)
	{
		return (int)-compb;
	}
	else
	{
		return -compa;
	}
}

void fu_print_file_data(const void* x)
{
	DEBUG(puts(__func__));
	printf("%s, %zu\n", (*((u_file_data**)x))->path, (*((u_file_data**)x))->data_len);
}

u_arr fu_evict_files(u_file_storage* storage, size_t bytes_to_free)
{
	DEBUG(puts(__func__));
	u_arr evict_arr = au_init_arr(storage->curr_files, sizeof(u_file_data*), fu_paths_compare, fu_print_file_data,
		NULL);
	hu_foreach(storage->table.table, storage->table.n_entries,
		au_insert(&evict_arr, &(hi->data));
	)

	qsort(evict_arr.data, evict_arr.element_count, evict_arr.element_size, fu_evict_compare);
	puts("/////////////////");

	au_print(&evict_arr);

	puts("/////////////////");

	size_t i = 0;
	printf("bytes_to_free = %zu\n", bytes_to_free);
	for (; i < evict_arr.element_count && bytes_to_free; i++)
	{
		if ((*((u_file_data**)(au_get(&evict_arr, i))))->data_len > bytes_to_free)
		{
			bytes_to_free = 0;
			continue;
		}
		bytes_to_free -= (*((u_file_data**)(au_get(&evict_arr, i))))->data_len;
		printf("bytes_to_free = %zu\n", bytes_to_free);
	}
	if (bytes_to_free > 0)
	{
		au_free(&evict_arr);
		return (u_arr){ 0 };
	}

	return evict_arr;
}
void fu_ensure_path(const char* path)
{
	DEBUG(puts(__func__));
	if (path == NULL)
	{
		fprintf(stdout, "fu_ensure_path : wrong params\n");
		fflush(stdout);
		return;
	}
	char temp_path[256] = { 0 }, * token, * save_ptr = NULL;
	strcpy(temp_path, path);
	size_t path_len = strlen(temp_path);
	while (path_len--)
	{
		if (temp_path[path_len] != '/')
		{
			temp_path[path_len] = 0;
		}
		else
		{
			break;
		}
	}

	char old_path[256] = { 0 };
	if (getcwd(old_path, 255) == NULL)
	{
		perror("getcwd at fu_ensure_path");
		errno = 0;
		return;
	}

	token = strtok_r(temp_path, "/", &save_ptr);
	if (chdir("/") == -1)
	{
		perror("chdir at fu_ensure_path");
		errno = 0;
		return;
	}

	while (token != NULL)
	{
		mkdir(token, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		printf("current token = [%s]\n", token);
		if (chdir(token) == -1)
		{
			perror("chdir at fu_ensure_path");
			errno = 0;
			return;
		}
		token = strtok_r(NULL, "/", &save_ptr);
	}
	errno = 0;
	if (chdir(old_path) == -1)
	{
		perror("chdir at fu_ensure_path");
		errno = 0;
		return;
	}
}