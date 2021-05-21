#include <server_fs.h>

int  sfs_init(sfs_fs *server_fs, int maxMB, int maxFiles)
{
    server_fs->currentFiles = 0;
    server_fs->currentSize = 0;
    server_fs->maxFiles = maxFiles;
    server_fs->maxSize = maxMB * 1024 * 1024;
    server_fs->filetable = calloc(maxFiles >> 2, sizeof(sfs_file *));

    return pthread_mutex_init(&(server_fs->lock), NULL);
}

retptr sfs_find(sfs_fs *server_fs, const char *name)
{  
    int hashpos = sfs_filetable_hash(name, server_fs->maxFiles >> 2);
    retptr ret = {0, NULL};

    if (server_fs->filetable[hashpos] == NULL)
    {
        ret.errorcodes = SFS_FILENOTFOUND;
        return ret;
    }
    sfs_file *curr = server_fs->filetable[hashpos];
    while (curr != NULL && strcmp(name, curr->name) > 0)
    {
        curr = curr->next;
    }
    if (curr == NULL)
    {
        ret.errorcodes = SFS_FILENOTFOUND;
    } else 
    {
        ret.ptr = curr;
    }

    return ret;
}

retptr sfs_open(sfs_fs *server_fs, const char *name, const char mode)
{
    if (server_fs == NULL || name == NULL) 
        return (retptr){1, NULL};
    
    sfs_file *file = (sfs_file *)sfs_find(server_fs, name).ptr;
    if (file == NULL)
        // file not fount
        return (retptr){1, NULL};

    pthread_mutex_lock(&(file->lock));

    if (file->finfo & SFS_FD_LOCK)
        // file locked
        return (retptr){1, NULL};
    if (file->finfo & SFS_FD_CLIENTS && mode == 'w')
        // file opened in readmode
        return (retptr){1, NULL};
    if ((file->finfo & SFS_FD_CLIENTS)==0b1111  
        && mode == 'r' || mode == 'R')
        // file opened in readmode by too many clients
        return (retptr){1, NULL};

    if (mode == 'R' || mode == 'w')
    {
        file->finfo |= SFS_FD_LOCK; // lockfile
    }
    file->finfo++; // add client
    file->finfo += 0b00010000; // add usage
    
    pthread_mutex_unlock(&(file->lock));
    sfs_fd *fd = (sfs_fd *)malloc(sizeof(sfs_fd));
    fd->file = file;
    fd->i = 0;

    return (retptr){0, fd};
}

int sfs_close(sfs_fs *server_fs, const char *name)
{
    return 0;
}

int sfs_write(sfs_fs *server_fs, sfs_fd *file, const void *data, const size_t size)
{
    return 0;
}

int sfs_read(sfs_fd *file, void *data, const size_t size)
{
    return 0;
}

int sfs_append(sfs_fs *server_fs, sfs_fd *file, const void *data, const size_t size)
{
    return 0;
}

retptr sfs_create(sfs_fs *server_fs, const char *name, const void *data, const size_t size)
{
    retptr ret;
    if ((ret = sfs_find(server_fs, name)).errorcodes == 0)
        return ret;

    if (server_fs->currentSize + size > server_fs->maxSize)
    {
        if (sfs_evict(server_fs, size))
            return (retptr){SFS_MEMORYFULL, NULL};
    }

    sfs_file *newFile = sfs_alloc_file(name, data, size);
    sfs_insert(server_fs, newFile);

    // continue
    return (retptr){0, (void *)newFile};
}

int sfs_remove(sfs_fs *server_fs, const char *name)
{
    retptr ret = sfs_find(server_fs, name);
    if (ret.errorcodes) return ret.errorcodes;
    sfs_file *file = (sfs_file *)ret.ptr;
    int hashpos = sfs_filetable_hash(file->name, server_fs->maxFiles >> 2);
    sfs_file *curr = server_fs->filetable[hashpos], *prev = NULL;

    while (curr != NULL && strcmp(file->name, curr->name) > 0)
    {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL) // remove head
    {
        server_fs->filetable[hashpos] = file->next;
    } else 
    {
        prev->next = file->next;
    }
    sfs_free_file(&file);

    return 0;
}

int sfs_evict(sfs_fs *server_fs, size_t size)
{
    size_t sizetofree = size + server_fs->currentSize - server_fs->maxSize;
    if (sizetofree <= 0)
        return SFS_WRONGCALL;

    // continue

    return 0;
}

int sfs_free_file(sfs_file **file)
{
    free((*file)->data);
    free((*file)->name);
    CHECK_COND_EXIT(
        pthread_mutex_destroy(&((*file)->lock)) != 0, 
        "sfs_free_file : pthread_mutex_destroy"
    )
    free(*file);
    *file = NULL;
    return 0;
}

sfs_file *sfs_alloc_file(const char name, const void *data, const size_t size)
{
    sfs_file *newFile;
    CHECK_BADVAL_PERROR_EXIT(
        newFile = (sfs_file *)malloc(sizeof(sfs_file)), 
        NULL, "sfs_create : malloc"
    );
    CHECK_BADVAL_PERROR_EXIT(
        newFile->data = (void *)malloc(size), 
        NULL, "sfs_create : malloc"
    );
    CHECK_BADVAL_PERROR_EXIT(
        newFile->name = (char *)malloc(strlen(name) + 1), 
        NULL, "sfs_create : malloc"
    );
    strcpy(newFile->name, name);
    memcpy(newFile->data, data, size);
    newFile->next = NULL;
    newFile->datalen = size;
    newFile->finfo = (unsigned char)0;
    CHECK_COND_EXIT(
        pthread_mutex_init(&(newFile->lock), NULL) != 0, 
        "sfs_create : pthread_mutex_init"
    )

    return newFile;
}

int sfs_insert(sfs_fs *server_fs, sfs_file *file)
{
    int hashpos = sfs_filetable_hash(file->name, server_fs->maxFiles >> 2);
    sfs_file *curr = server_fs->filetable[hashpos], *prev = NULL;
    while (curr != NULL && strcmp(file->name, curr->name) > 0)
    {
        prev = curr;
        curr = curr->next;
    }
    if (prev == curr)   // add first
    {
        server_fs->filetable[hashpos] = file;
    }
    else 
    {
        file->next = curr;
        if (prev != NULL) prev->next = file;
        else server_fs->filetable[hashpos] = file;
    }
    return 0;
}

int sfs_filetable_hash(const char *name, int nslots)
{
    int res = 0;
    char *temp = name;

    while(*temp)
    {
        res += (int) *(temp++);
    }

    return res % nslots;
}