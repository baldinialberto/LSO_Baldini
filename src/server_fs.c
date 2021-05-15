#include <server_fs.h>

server_fs init_server_fs(int maxCapacity, int maxFileCount)
{
    server_fs memory;
    memory.currentFileCapacity = maxFileCount;
    memory.currentMemoryCapacity = maxCapacity;
    return memory;
}
sfs_file* sfs_create(char *path, int size, void *buff)
{
    return NULL;
}
int sfs_delete(char *path)
{
    return 0;
}
int sfs_write(sfs_file *file_descriptor, void *buff, int len)
{
    return 0;
}
int sfs_read(sfs_file *file_descriptor, void *buff, int len)
{
    return 0;
}
sfs_file *sfs_open(char *path, int mode)
{
    return 0;
}
int sfs_close(sfs_file *file_descriptor)
{
    return 0;
}
int sfs_lock(sfs_file *file_descriptor)
{
    return 0;
}
int sfs_unlock(sfs_file *file_descriptor)
{
    return 0;
}
int sfs_seek(sfs_file *file_descriptor, int from, int offs)
{
    return 0;
}
