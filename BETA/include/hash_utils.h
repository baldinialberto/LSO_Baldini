#ifndef _HASH_UTILS_H
#define _HASH_UTILS_H

#define hu_foreach(table, do)

typedef struct _utils_hash_table
{
    u_list *table;
    size_t nentries;
    size_t datasize;
    size_t (*hashfunc)(void *key);
    int (*datacompare)(void *a, void *b);
    void (*dataprint)(void *data);
    void (*datafree)(void *data);
} u_hash_table;

u_hash_table hu_init(size_t nentries, size_t datasize, 
    size_t (*hashfunc)(void *key), int (*datacompare)(void *a, void *b), 
    void (*dataprint)(void *data), void (*datafree)(void *data));

int hu_ishere(u_hash_table *table, void *data);
void *hu_search(u_hash_table *table, void *data);
int hu_insert(u_hash_table *table, void *data);
int hu_insert_oncopy(u_hash_table *table, void *data);
int hu_remove(u_hash_table *table, void *data);
void hu_free(u_hash_table *table);
void hu_print(u_hash_table *table);

#endif