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
    struct _utils_list_node *next;
} u_list_node;
typedef u_list_node *u_list;

int lu_char_compare(void *a, void *b);
int lu_int_compare(void *a, void *b);
void lu_int_print(void *data);
int lu_string_compare(void *a, void *b);
void lu_string_print(void *data);

u_list lu_empty_list();
u_list_node *lu_allocnode(void *data);
u_list_node *lu_allocnode_oncopy(void *data, size_t size);
int lu_insert(u_list *list, void *data, int (*datacompare)(void *, void *));
int lu_insert_oncopy(u_list *list, void *data, size_t size, int (*datacompare)(void *, void *));
int lu_remove(u_list *list, void *data, int (*datacompare)(void *, void *), void (*datafree)(void *));
int lu_remove_node(u_list *list, u_list_node *node, int (*datacompare)(void *, void *), void (*datafree)(void *));
void _lu_navigate(u_list list, u_list_node **curr, u_list_node **prev, void *data, int (*datacompare)(void *, void *));
int lu_search(u_list *list, void *data, int (*datacompare)(void *, void *));
void *lu_get(u_list *list, void *data, int (*datacompare)(void *, void *));
int lu_length(u_list list);
int lu_free(u_list *list, void (*datafree)(void *));
int lu_print(u_list list, void (*dataprint)(void *));
void *lu_plainarr(u_list list, size_t datasize);
char *lu_tostring(u_list list);

#endif