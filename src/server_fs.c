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
    
    int requiredpages = sfs_pages_required_byfile(path, size);
    sfs_fd *newFile = NULL;

    if (requiredpages > fs->freepages) 
        sfs_evictpages(fs, requiredpages - fs->freepages);

    for (int i = 0; i < fs->pagecount; i++)
    {
        if (!((fs->memory)[i].pnode && SFS_PNODE_PRES_BIT))
        {
            (fs->memory)[i].pnode |= SFS_PNODE_PRES_BIT;
            (fs->memory)[i].pnode |= SFS_PNODE_INODE_BIT;
            if (requiredpages > 1)
                (fs->memory)[i].pnode |= SFS_PNODE_LINK_BIT;
            
        }
    }


}

int sfs_pages_required_byfile(char *path, size_t size)
{
    int requiredpages = 1;
    int pathlen = strlen(path) + 1;
    int filedeflen = pathlen + 1 + 4 + 1;

    if (filedeflen + size > SFS_PAGESPACE)
    {
        size -= (SFS_PAGESPACE_CONT - filedeflen - 1);
        requiredpages++;
        size -= SFS_PAGESPACE;
        requiredpages += size / SFS_PAGESPACE_CONT + 
            (size % SFS_PAGESPACE_CONT > 0);
    }

    return requiredpages;
}