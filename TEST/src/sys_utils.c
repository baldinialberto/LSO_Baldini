#include <sys_utils.h>
#include <limits.h>
#include <unistd.h>
#include <ptr_utils.h>

int su_read(int fd, void *buff, size_t size)
{
    ssize_t bytes_readen = 0;

    while ((bytes_readen = read(fd, pu_voidptr_increase(buff, bytes_readen), size)) > -1)
    {
        size -= bytes_readen;
        if (!bytes_readen) // eof
        {
            return -1;
        }
        if (!size) // read terminated
        {
            return 0;
        }
    }
    return -1;
}

int su_write(int fd, void *buff, size_t size)
{
    ssize_t bytes_written = 0;

    while ((bytes_written = read(fd, pu_voidptr_increase(buff, bytes_written), size)) > -1)
    {
        size -= bytes_written;
        if (!bytes_written) // eof
        {
            return -1;
        }
        if (!size) // read terminated
        {
            return 0;
        }
    }
    return -1;

    return 0;
}