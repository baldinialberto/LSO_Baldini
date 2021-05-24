#ifndef _LIST_UTILS_H
#define _LIST_UTILS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "mem_utils.h"

#define lu_foreach(list, listnode, do)      \
listnode = list;                            \
while (listnode != NULL)                    \
{                                           \
    do;                                     \
    listnode = listnode->next;              \
}                                           \

typedef struct _utils_list_node
{
    void *data;
    _utils_list_node *next;
} u_list_node;
typedef u_list_node *u_list;

u_list lu_empty_list();
int lu_insert(u_list *list, void *data, int (*datacompare)(void *, void *));
int lu_search(u_list *list, int (*datasearch)(void *));
int lu_length(u_list list);
int lu_free(u_list *list, void (*datafree)(void *));
int lu_print(u_list list, void (*dataprint)(void *));
char *lu_plainarr(u_list list, char* (dataextract)(void *));

#endif