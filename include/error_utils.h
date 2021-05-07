#ifndef _ERROR_UTILS
#define _ERROR_UTILS
 
#pragma once

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_VALUE(X, VALUE) assert(X == VALUE);
#define CHECK_NULL_RETURN(X, RET) \
if (!(X))           \
{                   \
    return(RET);    \
}
#define CHECK_BADVAL_PERROR_RETURN(X, BADVAL, MSG, RET) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
    errno = 0;      \
    return(RET);    \
}
#define CHECK_BADVAL_PERROR_EXIT(X, BADVAL, MSG) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
    errno = 0;      \
    exit(-1);       \
}
#define CHECK_BADVAL_PERROR(X, BADVAL, MSG) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
    errno = 0;      \
}
#define CHECK_BADVAL_CALL(X, BADVAL, FUN) \
if (X == BADVAL)    \
{                   \
   FUN;             \
}
#define CHECK_ERRNO_CALL(FUN, MSG) \
if (errno)          \
{                   \
    perror(MSG);    \
    errno = 0;      \
    FUN;            \
}
#define CHECK_ERRNO_RETURN(RET, MSG) \
if (errno)          \
{                   \
    perror(MSG);    \
    errno = 0;      \
    return RET;     \
}
#define CHECK_ERRNO_EXIT(XCODE, MSG) \
if (errno)          \
{                   \
    perror(MSG);    \
    errno = 0;      \
    exit(XCODE);     \
}
#define CHECK_COND_RETURN(COND, RET, MSG) \
if (COND) {         \
    perror(MSG);    \
    return RET;     \
}

#endif