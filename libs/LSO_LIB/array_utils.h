//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_ARRAY_UTILS_H
#define LSO_LIB_ARRAY_UTILS_H

#define size_t unsigned long

typedef struct u_arr
{
    void *data;
    size_t len;
    size_t element_count;
    size_t element_size;
    int (*compare)(void *, void *);
    void (*print)(void *);
} u_arr;

u_arr au_init_arr(size_t len, size_t element_size, int(*compare)(void *, void *));
void au_free(u_arr *arr);
int au_insert(u_arr *arr, void *new_element);
int au_remove(u_arr *arr, void *old_element);
int au_print(u_arr *arr);
void *au_get(u_arr *arr, size_t i);
void au_sort(u_arr *arr);
int au_extends(u_arr *arr, size_t new_len);
int au_shrinks(u_arr *arr);

#endif //LSO_LIB_ARRAY_UTILS_H
