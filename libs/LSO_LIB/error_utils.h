#ifndef LSO_LIB_ERROR_UTILS_H
#define LSO_LIB_ERROR_UTILS_H

#include <errno.h>

#define CHECK_PERROR_EXIT(TOCHECK, MSSG)            \
if (TOCHECK)                                        \
{                                                   \
    perror(MSSG);                                   \
    exit(-1);                                       \
}

#define CHECK_PERROR_RETURN(TOCHECK, MSSG, RETURN)  \
if (TOCHECK)                                        \
{                                                   \
    perror(MSSG);                                   \
    errno = 0;                                      \
    return RETURN;                                  \
}

#define CHECK_ERRNO_PERROR_EXIT(MSSG)               \
if (errno)                                          \
{                                                   \
    perror(MSSG);                                   \
    exit(-1);                                       \
}

#define CHECK_ERRNO_PERROR_RETURN(MSSG, RETURN)     \
if (errno)                                          \
{                                                   \
    perror(MSSG);                                   \
    errno = 0;                                      \
    return RETURN;                                  \
}

#endif //LSO_LIB_ERROR_UTILS_H
