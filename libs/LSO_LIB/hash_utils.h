//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_HASH_UTILS_H
#define LSO_LIB_HASH_UTILS_H

#pragma once
#include <stdlib.h>

#define hu_foreach(table, n_entries, do)                                \
for (size_t i = 0; i < (n_entries); i++)                                \
{                                                                       \
    for (u_hash_item *hi = (table)[i]; hi != NULL; hi = hi->next)       \
    {                                                                   \
        do;                                                             \
    }                                                                   \
}                                                                       \

typedef struct utils_hash_item {
	const void* key;
	void* data;
	struct utils_hash_item* next;
} u_hash_item;
typedef u_hash_item* u_hash_list;
typedef struct utils_hash_table {
	u_hash_list* table;
	size_t n_entries;
	size_t (* hash)(const void* key);
	int (* compare)(const void* a, const void* b);
	void (* print)(const void* data);
	void (* free_data)(void* data);
	void (* free_key)(void* data);
} u_hash_table;

u_hash_table hu_init(size_t n_entries, size_t (* hash_func)(const void* key),
		int (* compare_func)(const void* a, const void* b), void (* print_func)(const void* data),
		void (* free_data_func)(void* data), void (* free_key_func)(void* data));
u_hash_item* hu_alloc_item(const void* key, void* data);
int hu_insert(u_hash_table* table, const void* key, void* data);
int hu_remove(u_hash_table* table, const void* key);
void hu_navigate_list(u_hash_list list, u_hash_item** curr, u_hash_item** prev,
		const void* key, int (* compare)(const void*, const void*));
void* hu_get(u_hash_table* table, const void* key);
int hu_is_empty(u_hash_table* table);
void hu_free(u_hash_table* table);
void hu_print(u_hash_table* table);
size_t hu_index(u_hash_table* table, const void* key);

size_t hu_hash_string(const void* key);

#endif //LSO_LIB_HASH_UTILS_H
