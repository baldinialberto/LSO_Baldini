//
// Created by Alberto Baldini on 31/05/21.
//
#include "mem_utils.h"
#include "array_utils.h"
#include <stdio.h>
#include <stdlib.h>


u_arr au_init_arr(size_t len, size_t element_size, int(*compare)(void *, void *))
{
    if (len == 0 || element_size == 0)
    {
        fprintf(stderr, "au_init_arr : wrong params\n");
        fflush(stderr);
        return (u_arr){0};
    }
    u_arr new_arr;
    new_arr.len = len;
    new_arr.element_size = element_size;
    new_arr.compare = compare;
    new_arr.element_count = 0;
    new_arr.data = mu_calloc(len * element_size);
    return new_arr;
}
void au_free(u_arr *arr)
{

}
int au_insert(u_arr *arr, void *new_element)
{

}
int au_remove(u_arr *arr, void *old_element)
{

}
int au_print(u_arr *arr)
{

}
void *au_get(u_arr *arr, size_t i)
{

}
void au_sort(u_arr *arr)
{

}
int au_extends(u_arr *arr, size_t new_len)
{

}
int au_shrinks(u_arr *arr)
{

}