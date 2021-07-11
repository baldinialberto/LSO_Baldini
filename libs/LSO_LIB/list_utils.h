//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_LIST_UTILS_H
#define LSO_LIB_LIST_UTILS_H

#pragma once

#include <stdlib.h>

#define lu_foreach(list, to_do)        \
for (u_list_node *i = list->head; i != NULL; i = i->next)    \
{                                      \
    to_do;                             \
}

typedef struct utils_list_node {
	void* data;
	struct utils_list_node* next;
} u_list_node;
typedef struct utils_list {
	u_list_node* head;
	size_t len;
	size_t element_size;
	int (* compare)(const void*, const void*);
	void (* free)(void*);
	void (* print)(const void*);
} u_list;

u_list_node* lu_alloc_node(void* data);
u_list lu_init_list(size_t element_size, int (* compare_func)(const void*, const void*),
		void (* free_func)(void*), void (* print_func)(const void*));
int lu_insert(u_list* list, const void* data);
int lu_remove(u_list* list, const void* data);
void lu_navigate(u_list* list, u_list_node** curr, u_list_node** prev, const void* data);
long int lu_index_of_obj(u_list* list, const void* obj);
int lu_is_empty(u_list* list);
void* lu_get(u_list* list, long int index);
void lu_free(u_list* list);
void lu_print(u_list* list);
void* lu_plain_arr(u_list* list);
char* lu_tostring(u_list* list);

#endif //LSO_LIB_LIST_UTILS_H
