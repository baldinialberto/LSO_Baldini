#include <server_fs.h>

int init_server_fs(SFS_FS *fs, int avaiableMemory, int maxFileCount)
{
    // each file needs at least 1 page for storage 
    if (maxFileCount > (avaiableMemory * 1024)) return 1;

    fs->max_filecount = maxFileCount;
    fs->pagecount = (fs->freepages = avaiableMemory * 1024);
    CHECK_BADVAL_PERROR_EXIT(
        fs->memory = calloc(fs->pagecount, sizeof(SFS_PAGE)), 
        NULL, "init_server_fs : calloc"
    );

    return 0;
}

sfs_fd* sfs_create(SFS_FS *fs, char *path, void *buff, size_t size)
{
    if (fs == NULL || path == NULL || buff == NULL)
        return (sfs_fd*)-1;
    
    int pathlen = strlen(path) + 1;
    int filedeflen = pathlen + 1 + 4 + 1;
    int requiredpages = 1;
    int tempSize = size;
    sfs_fd *newFile = NULL;

    if (filedeflen + tempSize > SFS_PAGESPACE)
    {
        tempSize -= (SFS_PAGESPACE_CONT - filedeflen - 1);
        requiredpages++;
        tempSize -= SFS_PAGESPACE;
        requiredpages += tempSize / SFS_PAGESPACE_CONT + 
            (tempSize % SFS_PAGESPACE_CONT > 0);
    }

    if (requiredpages > fs->freepages) 
        sfs_evictpages(fs, requiredpages - fs->freepages);

    while (size)
    {
    }


}