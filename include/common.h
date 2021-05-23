#ifndef _COMMON_H
#define _COMMON_H

#pragma once
#include <stdlib.h>
#include "error_utils.h"

#define DEBUG(X) X


#define LIST_FOREACH(LIST, TEMP, DO)  \
TEMP = LIST;                    \
while (TEMP != NULL)            \
    {                           \
        DO;                     \
        TEMP = (LIST)->next;    \
    }                           \

typedef char bool;
typedef struct _retptr
{
    int errorcodes;
    void *ptr;
} retptr;
typedef struct _list_node
{
    void *data;
    struct _list_node *next;
} list_node;
typedef list_node *ln_ptr;

list_node *list_allocnode(void *data);
int list_insert(ln_ptr *list, list_node *node, int (*comp)(void*, void *));
int list_foreach(ln_ptr list, void (*proc)(void *));
int list_free(ln_ptr *list);

#endif