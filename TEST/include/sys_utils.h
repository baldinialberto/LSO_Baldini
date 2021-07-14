#ifndef SYS_UTILS_H
#define SYS_UTILS_H

#include <sys/types.h>

/**
 * Wrapper for function read
 * @param fd file descriptor to read from
 * @param buff buffer to read data to
 * @param size bytes to read
 * @return 0 if success -1 if error, errno is set accordingly
 */
int su_read(int fd, void *buff, size_t size);

/**
 * Wrapper for function write
 * @param fd file descriptor to write to
 * @param buff buffer to write data from
 * @param size bytes to write
 * @return 0 if success -1 if error, errno is set accordingly
 */
int su_write(int fd, void *buff, size_t size);



#endif