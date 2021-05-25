#include "../include/hash_utils.h"

u_hash_table hu_init(size_t nentries, size_t datasize, 
    size_t (*hashfunc)(void *key), int (*datacompare)(void *a, void *b), 
    void *(*extractkey)(void *data), 
    void (*dataprint)(void *data), void (*datafree)(void *data))
{
    u_hash_table table;
    table.nentries = nentries;
    table.datasize = datasize;
    table.hashfunc = hashfunc;
    table.datacompare = datacompare;
    table.extractkey = extractkey;
    table.dataprint = dataprint;
    table.datafree = datafree;
    table.table = mu_calloc(sizeof(u_list) * nentries);

    return table;
}
int hu_ishere(u_hash_table *table, void *key)
{
    return 0;
}
int hu_insert(u_hash_table *table, void *data)
{
    return 0;
}
int hu_insert_oncopy(u_hash_table *table, void *data)
{
    return 0;
}
int hu_remove(u_hash_table *table, void *key)
{
    return 0;
}
void *hu_get(u_hash_table *table, void *key)
{
    return NULL;
}
void hu_free(u_hash_table *table)
{

}
void hu_print(u_hash_table *table)
{

}