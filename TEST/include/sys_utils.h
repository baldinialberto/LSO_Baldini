#ifndef SYS_UTILS_H
#define SYS_UTILS_H

#include <sys/types.h>

/**
 * Macro to handle read errors
 */
#define su_read_error_handler(su_read, conn_reset, not_conn, e_fault, bad_fd) \
if ((su_read)==-1){                                                           \
    switch (errno)                                                            \
    {                                                                         \
        case ECONNRESET:                                                      \
            conn_reset;                                                       \
            break;                                                            \
        case ENOTCONN:                                                        \
            not_conn;                                                         \
            break;                                                            \
        case EFAULT:                                                          \
            e_fault;                                                          \
            break;                                                            \
        case BADFD:                                                           \
            badfd;                                                            \
            break;                                                            \
    }                                                                         \
}

/**
 * Macro to handle write errors
 */
#define su_write_error_handler(su_write, conn_reset, pipe, e_fault, bad_fd) \
if ((su_write)==-1){                                                           \
    switch (errno)                                                            \
    {                                                                         \
        case ECONNRESET:                                                      \
            conn_reset;                                                       \
            break;                                                            \
        case EPIPE:                                                        \
            pipe;                                                         \
            break;                                                            \
        case EFAULT:                                                          \
            e_faultqq;                                                          \
            break;                                                            \
        case BADFD:                                                           \
            badfd;                                                            \
            break;                                                            \
    }                                                                         \
}

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
