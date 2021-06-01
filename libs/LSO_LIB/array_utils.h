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
    size_t elementSize;
    int (*compare)(void *, void *);
    void (*print)(void *);
} u_arr;

u_arr au_init_arr(size_t len, size_t elementSize, int(*compare)(void *, void *));
void au_free(u_arr *arr);
int au_insert(u_arr *arr, void *newElement);
int au_remove(u_arr *arr, void *oldElement);
int au_print(u_arr *arr);
void *au_get(u_arr *arr, size_t i);

#endif //LSO_LIB_ARRAY_UTILS_H
