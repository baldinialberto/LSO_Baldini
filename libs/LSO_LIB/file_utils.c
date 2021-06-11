//
// Created by Alberto Baldini on 31/05/21.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_utils.h"
#include "hash_utils.h"
#include "file_utils.h"

void fu_storage_print(u_file_storage *file_storage)
{
    hu_print(&(file_storage->table));
}
size_t fu_storage_avBytes(u_file_storage *file_storage)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_storage_avBytes : param file_storage == NULL\n");
        fflush(stderr);
        return 0;
    }
    return file_storage->max_bytes - file_storage->n_bytes;
}
size_t fu_storage_avFiles(u_file_storage *file_storage)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_storage_avFiles : param file_storage == NULL\n");
        fflush(stderr);
        return 0;
    }
    return file_storage->max_files - file_storage->n_files;
}
u_file_storage fu_init_file_storage(size_t max_files, size_t max_bytes)
{
    if (max_files == 0)
    {
        fprintf(stderr, "fu_init_file_storage : param max_files == 0\n");
        fflush(stderr);
        return (u_file_storage){0};
    }
    if (max_bytes == 0)
    {
        fprintf(stderr, "fu_init_file_storage : param max_bytes == 0\n");
        fflush(stderr);
        return (u_file_storage){0};
    }
    u_file_storage filestorage;
    filestorage.n_files = 0;
    filestorage.max_files = max_files;
    filestorage.n_bytes = 0;
    filestorage.max_bytes = max_bytes;
    filestorage.table = hu_init(
            (filestorage.max_files >> 2) + 1,
            sizeof(u_file),
            hu_hash_string,
            fu_filenamecompare,
            fu_filepath,
            fu_filedata,
            fu_fileprint_hash,
            fu_filedatafree,
            mu_free
    );

    return filestorage;
}
u_file fu_allocfile(char *path, u_file_data *data)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile : param path == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }

    u_file newfile;
    newfile.data = data;
    newfile.data->data_info = 0;
    newfile.data->client = -1;
    pthread_mutex_init(&(newfile.data->mutex), NULL);
    newfile.path = path;

    return newfile;
}
u_file fu_allocfile_empty(char *path)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_empty : param path == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    return fu_allocfile_oncopy(path, NULL, 0);
}
u_file fu_allocfile_oncopy(char *path, const void *data, size_t data_len)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param path == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    if (data == NULL && data_len != 0)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param data == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    if (data != NULL && data_len == 0)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param data != NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    u_file_data *filedata = mu_calloc(sizeof(u_file_data));
    if (data != NULL)
    {
        filedata->data = mu_malloc(data_len);
        memcpy(filedata->data, data, data_len);
        filedata->data_len = data_len;
    }
    char *pathcopy = mu_malloc(strlen(path) + 1);
    strcpy(pathcopy, path);

    return fu_allocfile(pathcopy, filedata);
}
size_t fu_storage_files_available(u_file_storage *file_storage)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_storage_files_available : wrong params\n");
        fflush(stderr);
        return 0;
    }
    return file_storage->max_files - file_storage->n_files;
}
size_t fu_storage_bytes_available(u_file_storage *file_storage)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_storage_bytes_available : wrong params\n");
        fflush(stderr);
        return 0;
    }
    return file_storage->max_bytes - file_storage->n_bytes;
}
int fu_add_file(u_file_storage *file_storage, u_file_data *file, char *filepath)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_add_file : param file_storage == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file == NULL)
    {
        fprintf(stderr, "fu_add_file : param file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (fu_storage_avFiles(file_storage) == 0)
    {
        fprintf(stderr, "fu_add_file : file_storage has stored the maximum amount of files\n");
        fflush(stderr);
        return -1;
    }
    if (file->data_len > fu_storage_avBytes(file_storage))
    {
        fprintf(stderr, "fu_add_file : file_storage has not enough space to store this file\n");
        fflush(stderr);
        return -1;
    }
    if (hu_insert(&(file_storage->table), hu_alloc_item(filepath, file)))
    {
        fprintf(stderr, "fu_add_file : hu_insert has returned an error, file is not stored in file_storage\n");
        fflush(stderr);
        return -1;
    }
    (file_storage->n_files)++;
    file_storage->n_bytes += file->data_len;
    return 0;
}
int fu_remove_file(u_file_storage *file_storage, char *path)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_remove_file : param file_storage == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (path == NULL)
    {
        fprintf(stderr, "fu_remove_file : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (hu_ishere(&(file_storage->table), path))
    {
        fprintf(stderr, "fu_remove_file : path is not stored in storage\n");
        fflush(stderr);
        return -1;
    }
    size_t filedatalen = fu_getfile(file_storage, path)->data_len;
    if (hu_remove(&(file_storage->table), (void *)path))
    {
        fprintf(stderr, "fu_remove_file : hu_remove has returned an error, file is not removed from file_storage\n");
        fflush(stderr);
        return -1;
    }
    (file_storage->n_files)--;
    file_storage->n_bytes -= filedatalen;
    return 0;
}
u_file_data *fu_getfile(u_file_storage *file_storage, char *path)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_searchfile : param file_storage == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (path == NULL)
    {
        fprintf(stderr, "fu_searchfile : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return (u_file_data *)hu_get(&(file_storage->table), (void *)path);
}
void fu_storage_free(u_file_storage *file_storage)
{
    if (file_storage == NULL)
    {
        fprintf(stderr, "fu_storage_free : param file_storage == NULL\n");
        fflush(stderr);
        return;
    }
    hu_free(&(file_storage->table));
}
int fu_fd_read(const char *path, u_file_data *dest_file)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_fd_read : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (dest_file == NULL)
    {
        fprintf(stderr, "fu_fd_read : param dest_file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (dest_file->data != NULL && dest_file->data_len == 0)
    {
        fprintf(stderr, "fu_fd_read : dest_file has data in it. Free old data\n");
        fflush(stderr);
        free(dest_file->data);
    }
    if (dest_file->data == NULL && dest_file->data_len != 0)
    {
        fprintf(stderr, "fu_fd_read : dest_file has wrong data_len in it. Reset data_len\n");
        fflush(stderr);
        dest_file->data_len = 0;
    }
    if (dest_file->data != NULL && dest_file->data_len != 0)
    {
        fprintf(stderr, "fu_fd_read : dest_file has data in it. Abort\n");
        fflush(stderr);
        return -1;
    }
    FILE *file_to_read = fopen(path, "r");
    if (file_to_read == NULL)
    {
        fprintf(stderr, "fu_fd_read : unable to open %s\n", path);
        fflush(stderr);
        return -1;
    }
    size_t i = 0, datalen = 0, datachunk = 1024, readlen;
    char *data = mu_malloc(datachunk);
    while (!feof(file_to_read))
    {
        readlen = fread(data + i, 1, datachunk, file_to_read);
        if (ferror(file_to_read))
        {
            perror("fu_fd_read : fread");
            fprintf(stderr, "fu_fd_read : unable to read from %s\n", path);
            fflush(stderr);
            if (fclose(file_to_read))
            {
                perror("fu_fd_read : fclose");
                fprintf(stderr, "fu_fd_read : unable to close %s\n", path);
                fflush(stderr);
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
    } else
    {
        mu_free(data);
        data = NULL;
    }
    dest_file->data = data;
    dest_file->data_len = datalen;
    if (fclose(file_to_read))
    {
        fprintf(stderr, "fu_fd_read : unable to close %s\n", path);
        fflush(stderr);
        return -1;
    }
    return 0;
}
int fu_fd_write(u_file_data *file, const char *dest_path)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_fd_write : param file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file->data == NULL && file->data_len != 0)
    {
        fprintf(stderr, "fu_fd_write : param file->data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file->data_len == 0 && file->data != NULL)
    {
        fprintf(stderr, "fu_fd_write : param file->data_len == 0\n");
        fflush(stderr);
        return -1;
    }
    if (dest_path == NULL)
    {
        fprintf(stderr, "fu_fd_write : param dest_path == NULL\n");
        fflush(stderr);
        return -1;
    }
    FILE *file_to_write = fopen(dest_path, "w");
    if (file_to_write == NULL)
    {
        fprintf(stderr, "fu_fd_write : unable to open %s\n", dest_path);
        fflush(stderr);
        return -1;
    }
    fwrite(file->data, 1, file->data_len, file_to_write);
    if (ferror(file_to_write))
    {
        perror("fu_fd_write : fwrite");
        fprintf(stderr, "fu_fd_write : unable to write in %s\n", dest_path);
        fflush(stderr);
        if (fclose(file_to_write))
        {
            perror("fu_fd_write : fclose");
            fprintf(stderr, "fu_fd_write : unable to close %s\n", dest_path);
            fflush(stderr);
        }
        return -1;
    }
    if (fclose(file_to_write))
    {
        fprintf(stderr, "fu_fd_write : unable to close %s\n", dest_path);
        fflush(stderr);
        return -1;
    }
    return 0;
}
int fu_writefile(u_file_data *file, size_t i, void *data, size_t data_len)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_writefile : param file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "fu_writefile : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data_len == 0)
    {
        fprintf(stderr, "fu_writefile : param data_len == 0\n");
        fflush(stderr);
        return -1;
    }
    if (i > file->data_len)
    {
        fprintf(stderr, "fu_writefile : param i > file::data_len\n");
        fflush(stderr);
        return -1;
    }
    size_t additionalsize = (i + data_len) - file->data_len;
    if (additionalsize > 0)
    {
        file->data = mu_realloc(file->data, file->data_len + additionalsize);
    }
    memcpy((char *)(file->data) + i, data, data_len);

    return 0;
}
size_t fu_readfile(u_file_data *file, size_t i, void *dest, size_t dest_len)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_readfile : param file == NULL\n");
        fflush(stderr);
        return 0;
    }
    if (dest == NULL)
    {
        fprintf(stderr, "fu_readfile : param dest == NULL\n");
        fflush(stderr);
        return 0;
    }
    if (dest_len == 0)
    {
        fprintf(stderr, "fu_readfile : param dest_len == 0\n");
        fflush(stderr);
        return 0;
    }
    if (i >= file->data_len)
    {
        fprintf(stderr, "fu_readfile : param i >= file::data_len\n");
        fflush(stderr);
        return 0;
    }
    size_t remainingsize = file->data_len - i;
    if (dest_len > remainingsize) dest_len = remainingsize;
    memcpy(dest, (char *)(file->data) + i, dest_len);

    return dest_len;
}
size_t fu_readfile_realloc(u_file_data *file, size_t i, void **dest)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_readfile_realloc : param file == NULL\n");
        fflush(stderr);
        return 0;
    }
    if (dest == NULL)
    {
        fprintf(stderr, "fu_readfile_realloc : param dest == NULL\n");
        fflush(stderr);
        return 0;
    }
    if (i >= file->data_len)
    {
        fprintf(stderr, "fu_readfile_realloc : param i >= file::data_len\n");
        fflush(stderr);
        return 0;
    }
    size_t remainingsize = file->data_len - i;
    *dest = mu_realloc(*dest, remainingsize);
    return fu_readfile(file, i, *dest, remainingsize);
}
size_t fu_filehash(void *file)
{
    return 0;
}
int fu_filenamecompare(void *filenameA, void *filenameB)
{
    if (filenameA == NULL && filenameB == NULL)
    {
        fprintf(stderr, "fu_filepath : params fileA and fileB are NULL\n");
        fflush(stderr);
        return 0;
    }
    if (filenameA == NULL && filenameB != NULL)
    {
        fprintf(stderr, "fu_filepath : param fileA == NULL\n");
        fflush(stderr);
        return 1;
    }
    if (filenameB == NULL && filenameA != NULL)
    {
        fprintf(stderr, "fu_filepath : param fileB == NULL\n");
        fflush(stderr);
        return -1;
    }
    return strcmp((char *)filenameA, (char *)filenameB);
}
void *fu_filepath(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filepath : param file == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return (void *)(((u_file *)file)->path);
}
void *fu_filedata(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filedata : param file == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return ((u_file *)file)->data;
}
void fu_fileprint(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_fileprint : param file == NULL\n");
        fflush(stderr);
        return;
    }
    fprintf(stdout, "file %s, size %ld\n", (char*)fu_filepath(file), ((u_file_data *)fu_filedata(file))->data_len);
}
void fu_fileprint_hash(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_fileprint : param file == NULL\n");
        fflush(stderr);
        return;
    }
    u_hash_item *item = (u_hash_item *)file;
    fprintf(stdout, "file %s, size %ld\n", (char *)(item->key), ((u_file_data *)(item->data))->data_len);
}
void fu_filedatafree(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filefree : param file == NULL\n");
        fflush(stderr);
        return;
    }
    pthread_mutex_destroy(&(((u_file_data *)file)->mutex));
    mu_free(((u_file_data *)file)->data);
    mu_free((u_file_data *)file);
}
int fu_readpath(const char *path, void **data, size_t *data_len)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_readpath : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "fu_readpath : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data_len == NULL)
    {
        fprintf(stderr, "fu_readpath : param data_len == NULL\n");
        fflush(stderr);
        return -1;
    }
    u_file_data temp = {*data, *data_len, 0};
    if (fu_fd_read(path, &temp) == -1)
    {
        fprintf(stderr, "fu_readpath : fu_fd_read reaturned an error\n");
        fflush(stderr);
        return -1;
    }
    *data = temp.data;
    *data_len = temp.data_len;
    return 0;
}
int fu_writepath(const char *path, void *data, size_t data_len)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_writepath : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL && data_len != 0)
    {
        fprintf(stderr, "fu_writepath : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data != NULL && data_len == 0)
    {
        fprintf(stderr, "fu_writepath : param data_len == 0\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL && data_len == 0)
    {
        fprintf(stderr, "fu_writepath : nothing to do\n");
        fflush(stderr);
        return 0;
    }
    u_file_data temp = {data, data_len, 0};
    return fu_fd_write(&temp, path);
}
