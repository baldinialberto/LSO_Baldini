//
// Created by Alberto Baldini on 31/05/21.
//

#include "list_utils.h"
#include "mem_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

u_list lu_init_list(size_t element_size, int (*compare_func)(const void *, const void *),
                    void (*free_func)(void *), void (*print_func)(const void *))
{
    u_list new_list = {0};

    if (element_size != 0 && compare_func != NULL && print_func != NULL && free_func != NULL)
    {
        new_list.element_size = element_size;
        new_list.free = free_func;
        new_list.compare = compare_func;
        new_list.print = print_func;
    } else
    {
        fprintf(stderr, "lu_init_list : wrong params\n");
        fflush(stderr);
    }

    return new_list;
}
u_list_node *lu_alloc_node(void *data)
{
    if (data == NULL)
    {
        fprintf(stderr, "lu_alloc_node : wrong params\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *new_node = mu_malloc(sizeof(u_list_node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}
int lu_insert(u_list *list, const void *data)
{
    if (list == NULL || data == NULL)
    {
        fprintf(stderr, "lu_insert : wrong params\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev, *new_node = lu_alloc_node(mu_clone(data, list->element_size));
    lu_navigate(list, &curr, &prev, data);
    if (prev == NULL)
    {
        new_node->next = curr;
        list->head = new_node;
    }
    else
    {
        prev->next = new_node;
        new_node->next = curr;
    }
    list->len++;
    return 0;
}
int lu_remove(u_list *list, const void *data)
{
    if (list == NULL || data == NULL)
    {
        fprintf(stderr, "lu_remove : wrong params\n");
        fflush(stderr);
        return -1;
    }
    if (lu_is_empty(list))
    {
        fprintf(stderr, "lu_remove : list empty\n");
        fflush(stderr);
        return -1;
    }
    u_list_node *curr, *prev;
    lu_navigate(list, &curr, &prev, data);
    if (curr == NULL || list->compare(data, curr->data))
    {
        fprintf(stderr, "lu_remove : file_data not in list\n");
        fflush(stderr);
        return -1;
    } else
    {
        if (prev == NULL)
        {
            list->head = curr->next;

        } else
        {
            prev->next = curr->next;
        }
        list->free(curr->data);
        mu_free(curr);
        list->len--;
    }

    return 0;
}
void lu_navigate(u_list *list, u_list_node **curr, u_list_node **prev, const void *data)
{
    *curr = list->head;
    *prev = NULL;
    while (*curr != NULL && list->compare(data, (*curr)->data) > 0)
    {
        *prev = *curr;
        *curr = (*curr)->next;
    }
}
long int lu_index_of_obj(u_list *list, const void *obj)
{
    if (list == NULL || obj == NULL)
    {
        fprintf(stderr, "lu_index_of_obj : wrong params\n");
        fflush(stderr);
        return -1;
    }
    u_list_node  *curr = list->head;
    long int res = 0;
    while (curr != NULL && list->compare(obj, curr->data) > 0)
    {
        curr = (curr)->next;
        res++;
    }
    if (curr == NULL) return -1;
    return res;
}
int lu_is_empty(u_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_index_of_obj : wrong params\n");
        fflush(stderr);
        return -1;
    }
    return list->len == 0;
}
void *lu_get(u_list *list, long int index)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_get : wrong params\n");
        fflush(stderr);
        return NULL;
    }
    if (index < 0 || index >= list->len)
    {
        fprintf(stderr, "lu_get : param index out of range\n");
        fflush(stderr);
        return NULL;
    }
    u_list_node *curr = list->head;
    while (curr != NULL  && index > 0)
    {
        curr = curr->next;
        index--;
    }
    return curr == NULL ? NULL : curr->data;
}
void lu_free(u_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_free : wrong params\n");
        fflush(stderr);
        return;
    }
    if (lu_is_empty(list)) return;
    u_list_node *old_node, *curr_node = list->head;
    while (curr_node != NULL)
    {
        old_node = curr_node;
        curr_node = curr_node->next;
        list->free(old_node->data);
        mu_free(old_node);
    }
    list->head = NULL;
    list->len = 0;
}
void lu_print(u_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_print : wrong params\n");
        fflush(stderr);
        return;
    }
    if (!lu_is_empty(list)) {
        u_list_node *temp = list->head;
        while (temp != NULL)
        {
            list->print(temp->data);
            temp = temp->next;
        }
    }
    fprintf(stdout, "(NULL)\n");
    fflush(stdout);
}
void *lu_plain_arr(u_list *list)
{
    if (list == NULL)
    {
        fprintf(stderr, "lu_plainarr : wrong params\n");
        fflush(stderr);
        return NULL;
    }
    if (lu_is_empty(list)) return NULL;
    char *arr = mu_malloc(list->len * list->element_size);
    u_list_node *temp = list->head;
    size_t i = 0;
    while (temp != NULL)
    {
        memcpy(arr + (i * list->element_size), temp->data, list->element_size);
        temp = temp->next;
        i++;
    }
    return arr;
}
