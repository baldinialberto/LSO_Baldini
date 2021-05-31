//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_HASH_UTILS_H
#define LSO_LIB_HASH_UTILS_H

#define size_t unsigned long

#define hu_foreach(table, nentries, do) \
for (size_t i = 0; i < (nentries); i++) \
{   \
    for (u_hash_item *hi = (table)[i]; hi != NULL; hi = hi->next)   \
    {   \
        do; \
    }   \
}   \

typedef struct utils_hash_item
{
    void *key;
    void *data;
    struct utils_hash_item *next;
} u_hash_item;
typedef u_hash_item *u_hash_list;
typedef struct utils_hash_table
{
    u_hash_list *table;
    size_t nentries;
    size_t datasize;
    size_t (*hashfunc)(void *key);
    int (*keycompare)(void *a, void *b);
    void *(*extractkey)(void *data);
    void *(*extractdata)(void *data);
    void (*dataprint)(void *data);
    void (*datafree)(void *data);
    void (*keyfree)(void *data);
} u_hash_table;

size_t hu_hashstring(void *key);
u_hash_table hu_init(size_t nentries, size_t datasize,
                     size_t (*hashfunc)(void *key), int (*keycompare)(void *a, void *b),
                     void *(*extractkey)(void *data), void *(*extractdata)(void *data),
                     void (*dataprint)(void *data), void (*datafree)(void *data), void (*keyfree)(void *data));
u_hash_item *hu_allocitem(void *key, void *data);
u_hash_item *hu_allocitem_oncopy(void *key, void *data, size_t keylen, size_t datalen);
int hu_ishere(u_hash_table *table, void *key);
int hu_insert(u_hash_table *table, u_hash_item *item);
int hu_remove(u_hash_table *table, void *key);
void hu_navigate_list(u_hash_list list, u_hash_item **curr, u_hash_item **prev, void *key, int (*keycompare)(void *, void *));
void *hu_get(u_hash_table *table, void *key);
void hu_free(u_hash_table *table);
void hu_print(u_hash_table *table);
size_t hu_index(u_hash_table *table, void *key);

#endif //LSO_LIB_HASH_UTILS_H
