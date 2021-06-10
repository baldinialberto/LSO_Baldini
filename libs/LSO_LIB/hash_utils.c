//
// Created by Alberto Baldini on 31/05/21.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list_utils.h"
#include "string_utils.h"
#include "mem_utils.h"
#include "hash_utils.h"

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
    char c;

    while ((c = *(str++)))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
u_hash_table hu_init(size_t nentries, size_t datasize,
                     size_t (*hashfunc)(void *key), int (*keycompare)(void *a, void *b),
                     void *(*extractkey)(void *data), void *(*extractdata)(void *data),
                     void (*dataprint)(void *data), void (*datafree)(void *data),
                     void (*keyfree)(void *key))
{
    u_hash_table table;
    table.n_entries = nentries;
    table.data_size = datasize;
    table.hash = hashfunc;
    table.compare = keycompare;
    table.extract_key = extractkey;
    table.extract_data = extractdata;
    table.print = dataprint;
    table.free_data = datafree;
    table.free_key = keyfree;
    table.table = mu_calloc(sizeof(u_list) * nentries);

    return table;
}
u_hash_item *hu_alloc_item(void *key, void *data)
{
    if (key == NULL)
    {
        fprintf(stderr, "hu_alloc_item : param key == NULL\n");
        fflush(stderr);
        return NULL;
    }
    u_hash_item *newitem = mu_malloc(sizeof(u_hash_item));
    newitem->key = key;
    newitem->data = data;
    newitem->next = NULL;
    return newitem;
}
int hu_insert(u_hash_table *table, u_hash_item *data)
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
    u_hash_list *list = table->table + hu_index(table, data->key);
    u_hash_item *curr, *prev;
    hu_navigate_list(*list, &curr, &prev, data->key, table->compare);
    if (prev == NULL)
    {
        data->next = curr;
        *list = data;
    }
    else
    {
        prev->next = data;
        data->next = curr;
    }
    return 0;
}
int hu_remove(u_hash_table *table, void *key)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_remove : param table == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (key == NULL)
    {
        fprintf(stderr, "hu_remove : param key == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (table->table == NULL)
    {
        fprintf(stderr, "hu_remove : param table->table == NULL -> hashtable empty\n");
        fflush(stderr);
        return -1;
    }
    if (hu_ishere(table, key))
    {
        fprintf(stderr, "hu_remove : hu_ishere returned an error, data not in table\n");
        fflush(stderr);
        return -1;
    }
    u_hash_list *list = table->table + hu_index(table, key);
    u_hash_item *curr, *prev;
    hu_navigate_list(*list, &curr, &prev, key, table->compare);
    if (curr == NULL || table->compare(key, curr->key))
    {
        fprintf(stderr, "hu_remove : data not in list\n");
        fflush(stderr);
        fprintf(stdout, "hu_remove : data not in list\n");
        fflush(stdout);
        return -1;
    } else
    {
        if (prev == NULL)
        {
            *list = curr->next;

        } else
        {
            prev->next = curr->next;
        }
        table->free_data(curr->data);
        table->free_key(curr->key);
        free(curr);
    }
    return 0;
}
void *hu_get(u_hash_table *table, void *key)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_get : param table == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (table->table == NULL)
    {
        fprintf(stderr, "hu_get : param table->table == NULL -> hashtable empty\n");
        fflush(stderr);
        return NULL;
    }
    if (key == NULL)
    {
        fprintf(stderr, "hu_get : param key == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (hu_ishere(table, key))
    {
        fprintf(stderr, "hu_get : hu_ishere returned an error, data not in table\n");
        fflush(stderr);
        return NULL;
    }
    u_hash_list *list = table->table + hu_index(table, key);
    u_hash_item *curr, *prev;
    hu_navigate_list(*list, &curr, &prev, key, table->compare);
    if (curr == NULL || table->compare(key, curr->key))
    {
        fprintf(stderr, "hu_get : data not in list\n");
        fflush(stderr);
        fprintf(stdout, "hu_get : data not in list\n");
        fflush(stdout);
        return NULL;
    }
    return curr->data;
}
void hu_free(u_hash_table *table)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_free : param table == NULL\n");
        fflush(stderr);
        return;
    }
    if (table->table == NULL)
    {
        fprintf(stderr, "hu_free : param table->table == NULL -> hashtable empty\n");
        fflush(stderr);
        return;
    }
    u_hash_item *temp;
    for (size_t i = 0; i < table->n_entries; i++)
    {
        for (u_hash_item *hi = (table->table)[i]; hi != NULL;)
        {
            temp = hi;
            hi = hi->next;
            table->free_data(temp->data);
            table->free_key(temp->key);
            mu_free(temp);
        }
    }
    mu_free(table->table);
}
void hu_print(u_hash_table *table)
{
    puts("hu_print");
    if (table == NULL)
    {
        fprintf(stderr, "hu_print : param table == NULL\n");
        fflush(stderr);
        return;
    }
    if (table->table == NULL)
    {
        fprintf(stderr, "hu_print : param table->table == NULL -> hashtable empty\n");
        fflush(stderr);
        return;
    }
    hu_foreach(table->table, table->n_entries,
               puts("Hey");
                       table->print(hi)
    );
}
size_t hu_index(u_hash_table *table, void *key)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_index : param table == NULL\n");
        fflush(stderr);
        return 0;
    }
    if (key == NULL)
    {
        fprintf(stderr, "hu_index : param data == NULL\n");
        fflush(stderr);
        return 0;
    }
    return table->hash(key) % table->n_entries;
}
void hu_navigate_list(u_hash_list list, u_hash_item **curr, u_hash_item **prev, void *key, int (*keycompare)(void *, void *))
{
    *curr = list;
    *prev = NULL;
    while (*curr != NULL && keycompare(key, (void*)((*curr)->key)) > 0)
    {
        *prev = *curr;
        *curr = (*curr)->next;
    }
}
