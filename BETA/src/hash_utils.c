#include "../include/hash_utils.h"

size_t hu_hashstring(void *key)
{
    char *str = (char *)key;
    if (str == NULL)
    {
        fprintf(stderr, "hu_hashstring : param str == NULL\n");
        fflush(stderr);
        return 0;
    }

    // Dan Bernstein

    size_t hash = 5381;
    int c;

    while ((c = *(str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
u_hash_table hu_init(size_t nentries, size_t datasize, 
    size_t (*hashfunc)(void *key), int (*datacompare)(void *a, void *b), 
    void *(*extractkey)(void *data), void *(*extractdata)(void *data), 
    void (*dataprint)(void *data), void (*datafree)(void *data))
{
    u_hash_table table;
    table.nentries = nentries;
    table.datasize = datasize;
    table.hashfunc = hashfunc;
    table.datacompare = datacompare;
    table.extractkey = extractkey;
    table.extractdata = extractdata;
    table.dataprint = dataprint;
    table.datafree = datafree;
    table.table = mu_calloc(sizeof(u_list) * nentries);

    return table;
}
int hu_ishere(u_hash_table *table, void *key)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_ishere : param table == NULL\n");
        fflush(stderr);
        return -1;        
    }
    if (key == NULL)
    {
        fprintf(stderr, "hu_ishere : param key == NULL\n");
        fflush(stderr);
        return -1;        
    }
    if (table->table == NULL)
    {
        fprintf(stderr, "hu_ishere : param table->table == NULL -> hashtable empty\n");
        fflush(stderr);
        return -1;
    }

    return 0;
}
int hu_insert(u_hash_table *table, void *data)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_insert : param table == NULL\n");
        fflush(stderr);
        return -1;        
    }
    if (data == NULL)
    {
        fprintf(stderr, "hu_insert : param key == NULL\n");
        fflush(stderr);
        return -1;        
    }
    return 0;
}
int hu_insert_oncopy(u_hash_table *table, void *data)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_insert_oncopy : param table == NULL\n");
        fflush(stderr);
        return -1;        
    }
    if (data == NULL)
    {
        fprintf(stderr, "hu_insert_oncopy : param key == NULL\n");
        fflush(stderr);
        return -1;        
    }
    return 0;
}
int hu_remove(u_hash_table *table, void *data)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_remove : param table == NULL\n");
        fflush(stderr);
        return -1;        
    }
    if (data == NULL)
    {
        fprintf(stderr, "hu_remove : param key == NULL\n");
        fflush(stderr);
        return -1;        
    }
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