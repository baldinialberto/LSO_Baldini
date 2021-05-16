#include <server_fs.h>

int init_server_fs(SFS_FS *fs, int avaiableMemory, int maxFileCount)
{
    // each file needs 1/3 page for name, pos and size
    // and at least 1 page for storage 
    int minKB = (maxFileCount / 3) + 1 + maxFileCount;
    if (minKB > avaiableMemory * 1024) return 1;

    fs->max_filecount = maxFileCount;
    fs->pagecount = avaiableMemory * 1024;
    CHECK_BADVAL_PERROR_EXIT(
        fs->memory = calloc(fs->pagecount, sizeof(SFS_PAGE)), 
        NULL, "init_server_fs : calloc"
    );

    return 0;
}