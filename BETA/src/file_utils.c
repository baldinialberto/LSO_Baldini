#include "../include/file_utils.h"

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
        fu_filefree
    );

    return filestorage;
}
u_file* fu_allocfile(const char *path, const void *data, size_t datalen)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (data == NULL && datalen != 0)
    {
        fprintf(stderr, "fu_allocfile : param data == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (data != NULL && datalen == 0)
    {
        fprintf(stderr, "fu_allocfile : param datalen == 0\n");
        fflush(stderr);
        return NULL;
    }

    u_file *newfile = mu_malloc(sizeof(u_file));
    newfile->data = data;
    newfile->datalen = datalen;
    newfile->path = su_string_from_literal(path);

    return newfile;
}
u_file* fu_allocfile_empty(const char *path)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_empty : param path == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return fu_allocfile(path, NULL, 0);
}
u_file* fu_allocfile_oncopy(const char *path, const void *data, size_t datalen)
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
    void *datacopy = mu_malloc(datalen);
    memccpy(datacopy, data, 1, datalen);

    return fu_allocfile(path, datacopy, datalen);
}
void fu_freefile(u_file *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_freefile : param file == NULL\n");
        fflush(stderr);
    }
    su_free_string(&(file->path));
    free(file->data);
    free(file);
}
int fu_writefile(u_file *file, size_t i, void *data, size_t datalen)
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
size_t fu_readfile(u_file *file, size_t i, void *dest, size_t destlen)
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
size_t fu_readfile_realloc(u_file *file, size_t i, void **dest)
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
int fu_filecompare(void *fileA, void *fileB)
{
    if (fileA == NULL)
    {
        fprintf(stderr, "fu_filepath : param file == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (fileB == NULL)
    {
        fprintf(stderr, "fu_filepath : param file == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return strcmp(((u_file *)fileA)->path.data, ((u_file *)fileB)->path.data);
}
void *fu_filepath(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filepath : param file == NULL\n");
        fflush(stderr);
        return NULL;
    }
    return ((u_file *)file)->path.data;
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
    fprintf(stdout, "file %s, size %ld\n", (char*)fu_filepath(file), ((u_file *)file)->datalen);
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