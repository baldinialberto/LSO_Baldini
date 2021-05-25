#include "../include/file_utils.h"

size_t fu_storage_avBytes(u_file_storage *fstorage)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_storage_avBytes : param fstorage == NULL\n");
        fflush(stderr);
        return 0;
    }
    return fstorage->maxBytes - fstorage->nBytes;
}
size_t fu_storage_avFiles(u_file_storage *fstorage)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_storage_avFiles : param fstorage == NULL\n");
        fflush(stderr);
        return 0;
    }
    return fstorage->maxfiles - fstorage->nfiles;
}
u_file_storage fu_init_file_storage(size_t maxfiles, size_t maxBytes)
{
    if (maxfiles == 0)
    {
        fprintf(stderr, "fu_init_file_storage : param maxfiles == 0\n");
        fflush(stderr);
        return (u_file_storage){0};
    }
    if (maxBytes == 0)
    {
        fprintf(stderr, "fu_init_file_storage : param maxBytes == 0\n");
        fflush(stderr);
        return (u_file_storage){0};
    }
    u_file_storage filestorage;
    filestorage.nfiles = 0;
    filestorage.maxfiles = maxfiles;
    filestorage.nBytes = 0;
    filestorage.maxBytes = maxBytes;
    filestorage.table = hu_init(
        (filestorage.maxfiles >> 2) + 1, 
        sizeof(u_file), 
        hu_hashstring, 
        fu_filecompare, 
        fu_filepath, 
        fu_filedata, 
        fu_fileprint, 
        fu_filefree, 
        free
    );

    return filestorage;
}
u_file* fu_allocfile(char *path, u_file_data *data)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }

    u_file *newfile = mu_malloc(sizeof(u_file));
    newfile->data = data;
    newfile->data->datainfo = 0;
    newfile->path = path;

    return newfile;
}
u_file* fu_allocfile_empty(char *path)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_empty : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return fu_allocfile(path, NULL);
}
u_file* fu_allocfile_oncopy(char *path, const void *data, size_t datalen)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (data == NULL || datalen == 0)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param data == NULL\n");
        fflush(stderr);
        return NULL;
    }

    u_file_data *filedata = mu_malloc(sizeof(u_file_data));
    filedata->data = mu_malloc(datalen); 
    memccpy(filedata->data, data, 1, datalen);
    filedata->datalen = datalen;

    return fu_allocfile(path, filedata);
}
int fu_addfile(u_file_storage *fstorage, u_file *file)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_addfile : param fstorage == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file == NULL)
    {
        fprintf(stderr, "fu_addfile : param file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (fu_storage_avFiles(fstorage) == 0)
    {
        fprintf(stderr, "fu_addfile : fstorage has stored the maximum amount of files\n");
        fflush(stderr);
        return -1;
    }
    if (file->data->datalen > fu_storage_avBytes(fstorage))
    {
        fprintf(stderr, "fu_addfile : fstorage has not enough space to store this file\n");
        fflush(stderr);
        return -1;
    }
    
    if (hu_insert(&(fstorage->table), hu_allocitem(file->path, file->data)))
    {
        fprintf(stderr, "fu_addfile : hu_insert has returned an error, file is not stored in fstorage\n");
        fflush(stderr);
        return -1;
    }
    (fstorage->nfiles)++;
    fstorage->nBytes += file->data->datalen;
    return 0;
}
int fu_removefile(u_file_storage *fstorage, char *path)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_removefile : param fstorage == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (path == NULL)
    {
        fprintf(stderr, "fu_removefile : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (hu_ishere(&(fstorage->table), path))
    {
        fprintf(stderr, "fu_removefile : path is not stored in storage\n");
        fflush(stderr);
        return -1;
    }
    size_t filedatalen = fu_getfile(fstorage, path)->datalen;
    if (hu_remove(&(fstorage->table), (void *)path))
    {
        fprintf(stderr, "fu_removefile : hu_remove has returned an error, file is not removed from fstorage\n");
        fflush(stderr);
        return -1;
    }
    (fstorage->nfiles)--;
    fstorage->nBytes -= filedatalen;
    return 0;
}
u_file_data *fu_getfile(u_file_storage *fstorage, char *path)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_searchfile : param fstorage == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (path == NULL)
    {
        fprintf(stderr, "fu_searchfile : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return (u_file_data *)hu_get(&(fstorage->table), (void *)path);
}
void fu_storage_free(u_file_storage *fstorage)
{
    if (fstorage == NULL)
    {
        fprintf(stderr, "fu_storage_free : param fstorage == NULL\n");
        fflush(stderr);
        return;
    }
    hu_free(&(fstorage->table));
}
int fu_writefile(u_file_data *file, size_t i, void *data, size_t datalen)
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
    if (datalen == 0)
    {
        fprintf(stderr, "fu_writefile : param datalen == 0\n");
        fflush(stderr);
        return -1;
    }
    if (i > file->datalen)
    {
        fprintf(stderr, "fu_writefile : param i > file::datalen\n");
        fflush(stderr);
        return -1;
    }
    size_t additionalsize = (i + datalen) - file->datalen;
    if (additionalsize > 0)
    {
        file->data = mu_realloc(file->data, file->datalen + additionalsize);
    }
    memcpy((char *)(file->data) + i, data, datalen);

    return 0;
}
size_t fu_readfile(u_file_data *file, size_t i, void *dest, size_t destlen)
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
    if (destlen == 0)
    {
        fprintf(stderr, "fu_readfile : param destlen == 0\n");
        fflush(stderr);
        return 0;
    }
    if (i >= file->datalen)
    {
        fprintf(stderr, "fu_readfile : param i >= file::datalen\n");
        fflush(stderr);
        return 0;
    }
    size_t remainingsize = file->datalen - i;
    if (destlen > remainingsize) destlen = remainingsize;
    memcpy(dest, (char *)(file->data) + i, destlen);

    return destlen;
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
    if (i >= file->datalen)
    {
        fprintf(stderr, "fu_readfile_realloc : param i >= file::datalen\n");
        fflush(stderr);
        return 0;
    }
    size_t remainingsize = file->datalen - i;
    *dest = mu_realloc(*dest, remainingsize);
    return fu_readfile(file, i, *dest, remainingsize);
}
size_t fu_filehash(void *file)
{
    return 0;
}
int fu_filecompare(void *filenameA, void *filenameB)
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
    filenameA = (void *)(((u_hash_item *)filenameA)->key);
    filenameB = (void *)(((u_hash_item *)filenameB)->key);
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
    fprintf(stdout, "file %s, size %ld\n", (char*)fu_filepath(file), ((u_file_data *)fu_filedata(file))->datalen);
}
void fu_filefree(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filefree : param file == NULL\n");
        fflush(stderr);
        return;
    }
    su_free_string(fu_filepath(file));
    mu_free(fu_filedata(file));
    free(file);
}