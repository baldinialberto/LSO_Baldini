//
// Created by Alberto Baldini on 31/05/21.
//

#include "list_utils.h"
#include "mem_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lu_char_compare(void *a, void *b)
{
    if (a == NULL)
    {
        fprintf(stderr, "lu_char_compare : param a == 0\n");
        fflush(stderr);
        return b == NULL ? 0 : -1;
    }
    if (b == NULL)
    {
        fprintf(stderr, "lu_char_compare : param b == 0\n");
        fflush(stderr);
        return 1;
    }
    return *((char *)a) - *((char *)b);
}
int lu_int_compare(void *a, void *b)
{
    if (a == NULL)
    {
        fprintf(stderr, "lu_int_compare : param a == 0\n");
        fflush(stderr);
        return b == NULL ? 0 : -1;
    }
    if (b == NULL)
    {
        fprintf(stderr, "lu_int_compare : param b == 0\n");
        fflush(stderr);
        return 1;
    }
    return *((int *)a) - *((int *)b);
}
void lu_int_print(void *data)
{
    fprintf(stdout, "%d -> ", *((int *)data));
}
int lu_string_compare(void *a, void *b)
{
    if (a == NULL)
    {
        fprintf(stderr, "lu_string_compare : param a == 0\n");
        fflush(stderr);
        return b == NULL ? 0 : -1;
    }
    if (b == NULL)
    {
        fprintf(stderr, "lu_string_compare : param b == 0\n");
        fflush(stderr);
        return 1;
    }
    return strcmp((char *)a, (char *)b);
}
void lu_string_print(void *data)
{
    fprintf(stdout, "%s -> ", (char *)data);
}
u_list lu_empty_list()
{
    return NULL;
}
u_list_node *lu_alloc_node(void *data)
{
    if (data == NULL)
    {
        fprintf(stderr, "lu_allocnode : param data == NULL\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *newnode = mu_malloc(sizeof(u_list_node));
    newnode->data = data;
    newnode->next = NULL;
    return newnode;
}
u_list_node *lu_alloc_node_oncopy(void *data, size_t size)
{
    if (data == NULL)
    {
        fprintf(stderr, "lu_allocnode_oncopy : param data == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (size == 0)
    {
        fprintf(stderr, "lu_allocnode_oncopy : param size == 0\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *newnode = mu_malloc(sizeof(u_list_node));
    newnode->data = mu_malloc(size);
    memcpy(newnode->data, data, size);
    newnode->next = NULL;
    return newnode;
}
int lu_insert(u_list *list, void *data, int (*datacompare)(void *, void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_insert : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "lu_insert : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev, *newnode = lu_alloc_node(data);
    lu_navigate(*list, &curr, &prev, data, datacompare);
    if (prev == NULL)
    {
        newnode->next = curr;
        *list = newnode;
    }
    else
    {
        prev->next = newnode;
        newnode->next = curr;
    }
    return 0;
}
int lu_insert_oncopy(u_list *list, void *data, size_t size, int (*datacompare)(void *, void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_insert_oncopy : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "lu_insert_oncopy : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (size == 0)
    {
        fprintf(stderr, "lu_insert_oncopy : param size == 0\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev, *newnode = lu_alloc_node_oncopy(data, size);
    lu_navigate(*list, &curr, &prev, data, datacompare);
    if (prev == NULL)
    {
        newnode->next = curr;
        *list = newnode;
    }
    else
    {
        prev->next = newnode;
        newnode->next = curr;
    }
    return 0;
}
int lu_remove(u_list *list, void *data, int (*datacompare)(void *, void *), void (*datafree)(void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_remove : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "lu_remove : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (*list == NULL)
    {
        fprintf(stderr, "lu_remove : param *list == NULL -> list empty\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev;
    lu_navigate(*list, &curr, &prev, data, datacompare);
    if (curr == NULL || datacompare(data, curr->data))
    {
        fprintf(stderr, "lu_remove : data not in list\n");
        fflush(stderr);
        fprintf(stdout, "lu_remove : data not in list\n");
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
        datafree(curr->data);
        free(curr);
    }

    return 0;
}
int lu_remove_node(u_list *list, u_list_node *node, int (*datacompare)(void *, void *), void (*datafree)(void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_remove : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (node == NULL)
    {
        fprintf(stderr, "lu_remove : param node == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (*list == NULL)
    {
        fprintf(stderr, "lu_remove : param *list == NULL -> list empty\n");
        fflush(stderr);
        return -1;
    }
    return lu_remove(list, node->data, datacompare, datafree);
}
void lu_navigate(u_list list, u_list_node **curr, u_list_node **prev, void *data, int (*datacompare)(void *, void *))
{
    *curr = list;
    *prev = NULL;
    while (*curr != NULL && datacompare(data, (void*)((*curr)->data)) > 0)
    {
        *prev = *curr;
        *curr = (*curr)->next;
    }
}
int lu_search(u_list *list, void *data, int (*datacompare)(void *, void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_search : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (data == NULL)
    {
        fprintf(stderr, "lu_search : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (*list == NULL)
    {
        fprintf(stderr, "lu_search : param *list == NULL -> list empty\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev;
    lu_navigate(*list, &curr, &prev, data, datacompare);
    if (curr == NULL || datacompare(data, curr->data))
    {
        fprintf(stderr, "lu_search : data not in list\n");
        fflush(stderr);
        fprintf(stdout, "lu_search : data not in list\n");
        fflush(stdout);
        return -1;
    }
    return 0;
}
void *lu_get_by_index(u_list list, int index)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_get : param list == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (index < 0 || index >= lu_length(list))
    {
        fprintf(stderr, "lu_get : param index out of range\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *curr = list;
    while (curr != NULL  && index > 0)
    {
        curr = curr->next;
        index--;
    }
    return curr == NULL ? NULL : curr->data;
}
void *lu_get(u_list *list, void *data, int (*datacompare)(void *, void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_get : param list == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (data == NULL)
    {
        fprintf(stderr, "lu_get : param data == NULL\n");
        fflush(stderr);
        return NULL;
    }
    if (*list == NULL)
    {
        fprintf(stderr, "lu_get : param *list == NULL -> list empty\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *curr, *prev;
    lu_navigate(*list, &curr, &prev, data, datacompare);
    if (curr == NULL || datacompare(data, curr->data))
    {
        fprintf(stderr, "lu_get : data not in list\n");
        fflush(stderr);
        fprintf(stdout, "lu_get : data not in list\n");
        fflush(stdout);
        return NULL;
    }
    return curr->data;
}
int lu_length(u_list list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_length : param *list == NULL -> list empty\n");
        fflush(stderr);
        return 0;
    }
    int lenght = 0;
    while (list != NULL)
    {
        lenght++;
        list = list->next;
    }

    return lenght;
}
int lu_free(u_list *list, void (*datafree)(void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_free : param list == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (*list == NULL)
    {
        fprintf(stderr, "lu_search : param *list == NULL -> list empty\n");
        fflush(stderr);
        return 0;
    }
    u_list_node *oldnode;
    while (*list != NULL)
    {
        oldnode = *list;
        *list = (*list)->next;
        datafree(oldnode->data);
        free(oldnode);
    }

    return 0;
}
int lu_print(u_list list, void (*dataprint)(void *))
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_print : param list == NULL -> list empty\n");
        fflush(stderr);
        return 0;
    }
    u_list_node *temp;
    lu_foreach(list, temp, dataprint(temp->data));
    fprintf(stdout, "(NULL)\n");
    fflush(stdout);
    return 0;
}
void *lu_plainarr(u_list list, size_t datasize)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_plainarr : param list == NULL -> list empty\n");
        fflush(stderr);
        return NULL;
    }
    if (datasize == 0)
    {
        fprintf(stderr, "lu_plainarr : param datasize == NULL\n");
        fflush(stderr);
        return NULL;
    }
    size_t listlen = lu_length(list);
    char *arr = mu_malloc(listlen * datasize);
    size_t i = 0;
    u_list_node *temp;
    lu_foreach(list, temp,
               memcpy(arr + (i * datasize), temp->data, datasize);
    i++;
    );
    return arr;
}
char *lu_tostring(u_list list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_tostring : param list == NULL -> list empty\n");
        fflush(stderr);
        return NULL;
    }
    size_t listlen = lu_length(list);
    char *arr = mu_malloc(listlen + 1);
    size_t i = 0;
    u_list_node *temp;
    lu_foreach(list, temp,
               memcpy(arr + i, temp->data, sizeof(char));
    i++;
    );
    arr[i] = (char)0;
    return arr;
}