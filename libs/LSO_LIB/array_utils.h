//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_ARRAY_UTILS_H
#define LSO_LIB_ARRAY_UTILS_H

#pragma once
#include <stdlib.h>

typedef struct u_arr
{
    void *data;
    size_t len;
    size_t element_count;
    size_t element_size;
    int (*compare)(const void *, const void *);
    void (*print)(const void *);
    char is_sorted;
} u_arr;

u_arr au_init_arr(size_t len, size_t element_size, int(*compare)(const void *, const void *),
                  void (*print)(const void *));
void au_free(u_arr *arr);
int au_insert(u_arr *arr, const void *new_element);
int au_remove(u_arr *arr, const void *old_element);
void au_print(u_arr *arr);
void *au_get(u_arr *arr, size_t index);
long int au_index_of_obj(u_arr *arr, const void *obj);
void au_sort(u_arr *arr);
int au_extends(u_arr *arr, size_t new_len);
int au_shrinks(u_arr *arr);
int au_is_full(u_arr *arr);
int au_is_empty(u_arr *arr);

#endif //LSO_LIB_ARRAY_UTILS_H
