#ifndef _COMMON_H
#define _COMMON_H

#ifndef DEBUG
#define DEBUG(X) 
#else 
#define DEBUG(X) X
#endif

typedef char bool;
typedef struct _retptr
{
    int errorcodes;
    void *ptr;
} retptr;

#endif