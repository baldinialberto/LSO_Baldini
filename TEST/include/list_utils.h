//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_LIST_UTILS_H
#define LSO_LIB_LIST_UTILS_H

#pragma once

#include <stdlib.h>

#define lu_foreach(list, to_do)        \
for (u_list_node *i = list->head; i != NULL; i = i->next)    \
{                                      \
    to_do;                             \
}

typedef struct utils_list_node {
    void *data;
    struct utils_list_node *next;
} u_list_node;
typedef struct utils_list {
    u_list_node *head;
    size_t len;
    size_t element_size;
    int (*compare)(const void *, const void *);
    void (*free)(void *);
    void (*print)(const void *);
} u_list;

/**
 * Alloc a single list node
 * @param data pointer to allocated data
 * @return u_list_node* new allocated list node
 */
u_list_node *lu_alloc_node(void *data);

/**
 * Initiate a new list of the chosen type. Must specify methods to manage the desired type.
 * Each method will treat the element as a void* .
 * @param element_size size of each element of the list
 * @param compare_func function int()(const void*, const void*) to compare 2 nodes' values
 * @param free_func function void()(void *) to free an element
 * @param print_func  function void()(const void *) to print an element
 * @return u_list newly initialized list
 */
u_list lu_init_list(size_t element_size, int (*compare_func)(const void *, const void *),
                    void (*free_func)(void *), void (*print_func)(const void *));

/**
 * Insert a new element in list
 * @param list pre-initialized list of type X
 * @param data pointer to initialized memory of type X
 * @return 0 if successful, -1 otherwise
 */
int lu_insert(u_list *list, const void *data);

/**
 * Remove the desired element from list
 * @param list pre-initialized list of type X
 * @param data pointer to initialized memory of type X
 * @return 0 if successful, -1 otherwise
 */
int lu_remove(u_list *list, const void *data);

/**
 * *PRIVATE* Navigate list, looking for a specified node
 * @param list pre-initialized list of type X
 * @param curr pointer to u_list_node* (temp)
 * @param prev pointer to u_list_node* (temp)
 * @param data node to navigate to
 */
void lu_navigate(u_list *list, u_list_node **curr, u_list_node **prev, const void *data);

/**
 * Get the index of element pointed by data
 * @param list pre-initialized list of type X
 * @param data pointer to desired element's index
 * @return -1 if data is not in list, a positive long otherwise
 */
long int lu_index(u_list *list, const void *data);

/**
 * Check if list is empty
 * @param list pre-initialized list of type X
 * @return 1 if is empty, 0 if is not, -1 if error
 */
int lu_is_empty(u_list *list);

/**
 * Get list's element at specified index
 * @param list pre-initialized list of type X
 * @param index of desired element
 * @return NULL if errors, a pointer to the desired
 * element if successful
 */
void *lu_get(u_list *list, long int index);

/**
 * Free list using the specified method assigned
 * during list initialization
 * @param list pre-initialized list to free
 */
void lu_free(u_list *list);

/**
 * Print list using the specified method assigned
 * during list initialization
 * @param list pre-initialized list to print
 */
void lu_print(u_list *list);

/**
 * Get a newly allocated array containing every element in list
 * @param list pre-initialized list
 * @return NULL if errors, a pointer to the new array otherwise
 */
void *lu_plain_arr(u_list *list);

/**
 * *NOT_IMPLEMENTED* Get a newly allocated string containing
 * every element in list
 * @param list pre-initialized list
 * @return NULL if error, a pointer to the new string otherwise
 */
char *lu_tostring(u_list *list);

#endif //LSO_LIB_LIST_UTILS_H
