#ifndef _ERRORS_H
#define _ERRORS_H
 
#pragma once

#include <assert.h>

#define CHECK_NULL_RETURN(X, RET) if (!(X)) { return(RET); }
#define ASSERT_VALUE(X, VALUE) assert(X == VALUE);
#define CHECK_BADVAL_PERROR_RETURN(X, BADVAL, MSG, RET) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
    return(RET)     \
}
#define CHECK_BADVAL_PERROR_EXIT(X, BADVAL, MSG) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
    exit(-1)        \
}
#define CHECK_BADVAL_PERROR(X, BADVAL, MSG) \
if (X == BADVAL)    \
{                   \
    perror(MSG);    \
}
#define CHECK_BADVAL_CALL(X, BADVAL, FUN) \
if (X == BADVAL)    \
{                   \
   FUN;             \
}

#endif