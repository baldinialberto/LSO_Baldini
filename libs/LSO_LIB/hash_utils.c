//
// Created by Alberto Baldini on 31/05/21.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem_utils.h"
#include "hash_utils.h"

size_t hu_hash_string(const void *key)
{
    char *str = (char *)key;
    if (str == NULL)
    {
        fprintf(stderr, "hu_hash_string : param str == NULL\n");
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
u_hash_table hu_init(size_t n_entries, size_t (*hash_func)(const void *key),
                     int (*compare_func)(const void *a, const void *b), void (*print_func)(const void *data),
                     void (*free_data_func)(void *data), void (*free_key_func)(void *data))
{
    u_hash_table table;
    table.n_entries = n_entries;
    table.hash = hash_func;
    table.compare = compare_func;
    table.print = print_func;
    table.free_data = free_data_func;
    table.free_key = free_key_func;
    table.table = mu_calloc(sizeof(u_hash_list) * n_entries);

    return table;
}
u_hash_item *hu_alloc_item(const void *key, void *data)
{
    if (key == NULL)
    {
        fprintf(stderr, "hu_alloc_item : param key == NULL\n");
        fflush(stderr);
        return NULL;
    }
    u_hash_item *new_item = mu_malloc(sizeof(u_hash_item));
    new_item->key = key;
    new_item->data = data;
    new_item->next = NULL;
    return new_item;
}
int hu_insert(u_hash_table *table, const void *key, void *data)
{
    if (table == NULL || data == NULL)
    {
        fprintf(stderr, "hu_insert : param table == NULL\n");
        fflush(stderr);
        return -1;
    }
    u_hash_list *list = table->table + hu_index(table, key);
    u_hash_item *curr, *prev, *new_item = hu_alloc_item(key, data);
    hu_navigate_list(*list, &curr, &prev, key, table->compare);
    if (prev == NULL)
    {
        new_item->next = curr;
        *list = new_item;
    }
    else
    {
        prev->next = new_item;
        new_item->next = curr;
    }
    return 0;
}
int hu_remove(u_hash_table *table, const void *key)
{
    if (table == NULL || key == NULL)
    {
        fprintf(stderr, "hu_remove : wrong params\n");
        fflush(stderr);
        return -1;
    }
    if (hu_is_empty(table))
    {
        fprintf(stderr, "hu_remove : hashtable is empty\n");
        fflush(stderr);
        return -1;
    }

    u_hash_list *list = table->table + hu_index(table, key);
    u_hash_item *curr, *prev;
    hu_navigate_list(*list, &curr, &prev, key, table->compare);
    if (curr == NULL || table->compare(key, curr->key))
    {
        fprintf(stderr, "hu_remove : file_data not in list\n");
        fflush(stderr);
        fprintf(stdout, "hu_remove : file_data not in list\n");
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
        table->free_key((void *)curr->key);
        free(curr);
    }
    return 0;
}
void *hu_get(u_hash_table *table, const void *key)
{
    if (table == NULL || key == NULL)
    {
        fprintf(stderr, "hu_get : wrong params\n");
        fflush(stderr);
        return NULL;
    }
    if (hu_is_empty(table))
    {
        fprintf(stderr, "hu_get : hashtable is empty\n");
        fflush(stderr);
        return NULL;
    }

    u_hash_list *list = table->table + hu_index(table, key);
    u_hash_item *curr, *prev;
    hu_navigate_list(*list, &curr, &prev, key, table->compare);
    if (curr == NULL || table->compare(key, curr->key))
    {
        fprintf(stderr, "hu_get : file_data not in list\n");
        fflush(stderr);
        fprintf(stdout, "hu_get : file_data not in list\n");
        fflush(stdout);
        return NULL;
    }
    return curr->data;
}
int hu_is_empty(u_hash_table *table)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_is_empty : wrong params\n");
        fflush(stderr);
        return -1;
    }
    int empty = 1;
    u_hash_list list = NULL;

    for (size_t i = 0; i < table->n_entries && empty; i++)
    {
        list = table->table[i];
        if (list != NULL)
        {
            empty = 0;
        }
    }

    return empty;
}
void hu_free(u_hash_table *table)
{
    if (table == NULL)
    {
        fprintf(stderr, "hu_free : wrong params\n");
        fflush(stderr);
        return;
    }
    if (!hu_is_empty(table))
    {
        u_hash_item *temp;
        for (size_t i = 0; i < table->n_entries; i++)
        {
            for (u_hash_item *hi = (table->table)[i]; hi != NULL;)
            {
                temp = hi;
                hi = hi->next;
                table->free_data(temp->data);
                table->free_key((void *)temp->key);
                mu_free(temp);
            }
        }
    }
    mu_free(table->table);
}
void hu_print(u_hash_table *table)
{
    puts("hu_print");
    if (table == NULL)
    {
        fprintf(stderr, "hu_print : wrong params\n");
        fflush(stderr);
        return;
    }
    if (hu_is_empty(table))
    {
        fprintf(stderr, "hu_print : hashtable is empty\n");
        fflush(stderr);
        return;
    }
    hu_foreach(table->table, table->n_entries,
                       table->print(hi)
    );
}
size_t hu_index(u_hash_table *table, const void *key)
{
    if (table == NULL || key == NULL)
    {
        fprintf(stderr, "hu_index : wrong params\n");
        fflush(stderr);
        return 0;
    }
    return table->hash(key) % table->n_entries;
}
void hu_navigate_list(u_hash_list list, u_hash_item **curr, u_hash_item **prev, const void *key, int (*compare)(const void *, const void *))
{
    if (curr == NULL || prev == NULL || key == NULL || compare == NULL)
    {
        fprintf(stderr, "hu_navigate_list : wrong params\n");
        fflush(stderr);
        return;
    }
    *curr = list;
    *prev = NULL;
    while (*curr != NULL && compare(key, (*curr)->key) > 0)
    {
        *prev = *curr;
        *curr = (*curr)->next;
    }
}
