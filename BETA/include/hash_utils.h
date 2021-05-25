#ifndef _HASH_UTILS_H
#define _HASH_UTILS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "mem_utils.h"
#include "list_utils.h"

#define hu_foreach(table, do)

typedef struct _utils_hash_table
{
    u_list *table;
    size_t nentries;
    size_t datasize;
    size_t (*hashfunc)(void *key);
    int (*datacompare)(void *a, void *b);
    void *(*extractkey)(void *data);
    void (*dataprint)(void *data);
    void (*datafree)(void *data);
} u_hash_table;

u_hash_table hu_init(size_t nentries, size_t datasize, 
    size_t (*hashfunc)(void *key), int (*datacompare)(void *a, void *b), 
    void *(*extractkey)(void *data), 
    void (*dataprint)(void *data), void (*datafree)(void *data));
int hu_ishere(u_hash_table *table, void *key);
int hu_insert(u_hash_table *table, void *data);
int hu_insert_oncopy(u_hash_table *table, void *data);
int hu_remove(u_hash_table *table, void *key);
void *hu_get(u_hash_table *table, void *key);
void hu_free(u_hash_table *table);
void hu_print(u_hash_table *table);

#endif