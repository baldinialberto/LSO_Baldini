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
        fu_filenamecompare, 
        fu_filepath, 
        fu_filedata, 
        fu_fileprint, 
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
    newfile.data->datainfo = 0;
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
u_file fu_allocfile_oncopy(char *path, const void *data, size_t datalen)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param path == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    if (data == NULL && datalen != 0)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param data == NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    if (data != NULL && datalen == 0)
    {
        fprintf(stderr, "fu_allocfile_oncopy : param data != NULL\n");
        fflush(stderr);
        return (u_file){NULL};
    }
    u_file_data *filedata = mu_calloc(sizeof(u_file_data));
    if (data != NULL)
    {
        filedata->data = mu_malloc(datalen); 
        memcpy(filedata->data, data, datalen);
        filedata->datalen = datalen;
    }
    char *pathcopy = mu_malloc(strlen(path) + 1);
    strcpy(pathcopy, path);

    return fu_allocfile(pathcopy, filedata);
}
int fu_addfile(u_file_storage *fstorage, u_file_data *file, char *filepath)
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
    if (file->datalen > fu_storage_avBytes(fstorage))
    {
        fprintf(stderr, "fu_addfile : fstorage has not enough space to store this file\n");
        fflush(stderr);
        return -1;
    }
    
    if (hu_insert(&(fstorage->table), hu_allocitem(filepath, file)))
    {
        fprintf(stderr, "fu_addfile : hu_insert has returned an error, file is not stored in fstorage\n");
        fflush(stderr);
        return -1;
    }
    (fstorage->nfiles)++;
    fstorage->nBytes += file->datalen;
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
int fu_fread(const char *path, u_file_data *destfile)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_fread : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (destfile == NULL)
    {
        fprintf(stderr, "fu_fread : param destfile == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (destfile->data != NULL && destfile->datalen == 0)
    {
        fprintf(stderr, "fu_fread : destfile has data in it. Free old data\n");
        fflush(stderr);
        free(destfile->data);
    }
    if (destfile->data == NULL && destfile->datalen != 0)
    {
        fprintf(stderr, "fu_fread : destfile has wrong datalen in it. Reset datalen\n");
        fflush(stderr);
        destfile->datalen = 0;
    }
    if (destfile->data != NULL && destfile->datalen != 0)
    {
        fprintf(stderr, "fu_fread : destfile has data in it. Abort\n");
        fflush(stderr);
        return -1;
    }
    FILE *file_to_read = fopen(path, "r");
    if (file_to_read == NULL)
    {
        fprintf(stderr, "fu_fread : unable to open %s\n", path);
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
            perror("fu_fread : fread");
            fprintf(stderr, "fu_fread : unable to read from %s\n", path);
            fflush(stderr);
            if (fclose(file_to_read))
            {
                perror("fu_fread : fclose");
                fprintf(stderr, "fu_fread : unable to close %s\n", path);
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
    destfile->data = data;
    destfile->datalen = datalen;
    if (fclose(file_to_read))
    {
        fprintf(stderr, "fu_fread : unable to close %s\n", path);
        fflush(stderr);
        return -1;
    }
    return 0;
}
int fu_fwrite(u_file_data *file, const char *destpath)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_fwrite : param file == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file->data == NULL && file->datalen != 0)
    {
        fprintf(stderr, "fu_fwrite : param file->data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (file->datalen == 0 && file->data != NULL)
    {
        fprintf(stderr, "fu_fwrite : param file->datalen == 0\n");
        fflush(stderr);
        return -1;
    }
    if (destpath == NULL)
    {
        fprintf(stderr, "fu_fwrite : param destpath == NULL\n");
        fflush(stderr);
        return -1;
    }
    FILE *file_to_write = fopen(destpath, "w");
    if (file_to_write == NULL)
    {
        fprintf(stderr, "fu_fwrite : unable to open %s\n", destpath);
        fflush(stderr);
        return -1;
    }
    fwrite(file->data, 1, file->datalen, file_to_write);
    if (ferror(file_to_write))
    {
        perror("fu_fwrite : fwrite");
        fprintf(stderr, "fu_fwrite : unable to write in %s\n", destpath);
        fflush(stderr);
        if (fclose(file_to_write))
        {
            perror("fu_fwrite : fclose");
            fprintf(stderr, "fu_fwrite : unable to close %s\n", destpath);
            fflush(stderr);
        }
        return -1;
    }
    if (fclose(file_to_write))
    {
        fprintf(stderr, "fu_fwrite : unable to close %s\n", destpath);
        fflush(stderr);
        return -1;
    }
    return 0;
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
    fprintf(stdout, "file %s, size %ld\n", (char*)fu_filepath(file), ((u_file_data *)fu_filedata(file))->datalen);
}
void fu_filedatafree(void *file)
{
    if (file == NULL)
    {
        fprintf(stderr, "fu_filefree : param file == NULL\n");
        fflush(stderr);
        return;
    }
    mu_free(((u_file_data *)file)->data);
    mu_free((u_file_data *)file);
}
int fu_readpath(const char *path, void **data, size_t *datalen)
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
    if (datalen == NULL)
    {
        fprintf(stderr, "fu_readpath : param datalen == NULL\n");
        fflush(stderr);
        return -1;
    }
    u_file_data temp = {*data, *datalen, 0};
    if (fu_fread(path, &temp) == -1)
    {
        fprintf(stderr, "fu_readpath : fu_fread reaturned an error\n");
        fflush(stderr);
        return -1;
    }
    *data = temp.data;
    *datalen = temp.datalen;
    return 0;
}
int fu_writepath(const char *path, void *data, size_t datalen)
{
    if (path == NULL)
    {
        fprintf(stderr, "fu_writepath : param path == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL && datalen != 0)
    {
        fprintf(stderr, "fu_writepath : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data != NULL && datalen == 0)
    {
        fprintf(stderr, "fu_writepath : param datalen == 0\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL && datalen == 0)
    {
        fprintf(stderr, "fu_writepath : nothing to do\n");
        fflush(stderr);
        return 0;
    }
    u_file_data temp = {data, datalen, 0};
    return fu_fwrite(&temp, path);
}