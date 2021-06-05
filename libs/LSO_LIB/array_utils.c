//
// Created by Alberto Baldini on 31/05/21.
//
#include "mem_utils.h"
#include "array_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

u_arr au_init_arr(size_t len, size_t element_size, int(*compare)(const void *, const void *), void (*print)(const void *))
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
    new_arr.print = print;
    new_arr.element_count = 0;
    new_arr.data = mu_calloc(len * element_size);
    new_arr.is_sorted = 1;

    return new_arr;
}
void au_free(u_arr *arr)
{
    if (arr == NULL)
    {
        fprintf(stderr, "au_free : wrong params\n");
        fflush(stderr);
        return;
    }

    mu_free(arr->data);
    memset(arr, 0, sizeof(u_arr));
}
int au_insert(u_arr *arr, const void *new_element)
{
    if (arr == NULL || new_element == NULL)
    {
        fprintf(stderr, "au_insert : wrong params\n");
        fflush(stderr);
        return 1;
    }

    if (au_is_full(arr))
    {
        fprintf(stdout, "au_insert :  array is full\n");
        fflush(stdout);
        return 1;
    }
    memcpy(au_get(arr, arr->element_count), new_element, arr->element_size);
    arr->element_count++;
    arr->is_sorted = 0;

    return 0;
}
int au_remove(u_arr *arr, const void *old_element)
{
    if (arr == NULL || old_element == NULL)
    {
        fprintf(stderr, "au_remove : wrong params\n");
        fflush(stderr);
        return 1;
    }
    if (au_is_empty(arr))
    {
        fprintf(stdout, "au_remove : array is empty\n");
        fflush(stdout);
        return 1;
    }
    long i = au_index_of_obj(arr, old_element);
    if (i == -1)
    {
        fprintf(stdout, "au_remove : old_element not in array\n");
        fflush(stdout);
        return 1;
    }
    memcpy(au_get(arr, i), au_get(arr, arr->element_count-1), arr->element_size);
    memset(au_get(arr, arr->element_count-1), 0, arr->element_size); // optional
    arr->element_count--;
    arr->is_sorted = 0;

    return 0;
}
void au_print(u_arr *arr)
{
    if (arr == NULL || arr->print == NULL)
    {
        fprintf(stderr, "au_print : wrong params\n");
        fflush(stderr);
        return;
    }
    for (size_t i = 0; i < arr->element_count; i++)
    {
        arr->print(au_get(arr, i));
    }
}
void *au_get(u_arr *arr, size_t index)
{
    if (arr == NULL)
    {
        fprintf(stderr, "au_get : wrong params\n");
        fflush(stderr);
        return NULL;
    }
    if (index >= arr->len)
    {
        fprintf(stdout, "au_get : index %zu out of bounds\n", index);
        fflush(stdout);
        return NULL;
    }
    return (void *)((unsigned long)(arr->data) + (unsigned long)(index * arr->element_size));
}
long int au_index_of_obj(u_arr *arr, const void *obj)
{
    if (arr == NULL || obj == NULL)
    {
        fprintf(stderr, "au_get : wrong params\n");
        fflush(stderr);
        return -1;
    }
    void *ret_ptr;
    if (arr->is_sorted)
    {
        ret_ptr = bsearch(obj, arr->data, arr->element_count, arr->element_size, arr->compare);
    }
    else
    {
        ret_ptr = lfind(obj, arr->data, &(arr->element_count), arr->element_size, arr->compare);
    }
    return ret_ptr != NULL ? ((long)ret_ptr - (long)arr->data) / (long)arr->element_size : -1;
}
void au_sort(u_arr *arr)
{
    if (arr == NULL || arr->compare == NULL)
    {
        fprintf(stderr, "au_sort : wrong params\n");
        fflush(stderr);
        return;
    }
    if (arr->is_sorted)
    {
        fprintf(stdout, "au_sort : array is already sorted\n");
        fflush(stdout);
        return;
    }
    arr->is_sorted = (char)!heapsort(arr->data, arr->element_count, arr->element_size, arr->compare);
    if (!arr->is_sorted) perror("au_sort : heapsort");
}
int au_extends(u_arr *arr, size_t new_len)
{
    if (arr == NULL || new_len == 0)
    {
        fprintf(stderr, "au_extends : wrong params\n");
        fflush(stderr);
        return 1;
    }

    if (new_len < arr->element_count) arr->element_count = new_len;
    arr->data = mu_realloc(arr->data, new_len * arr->element_size);
    arr->len = new_len;

    return 0;
}
int au_shrinks(u_arr *arr)
{
    if (arr == NULL)
    {
        fprintf(stderr, "au_shrinks : wrong params\n");
        fflush(stderr);
        return 1;
    }

    if (arr->element_count == arr->len)
    {
        fprintf(stdout, "au_shrinks : element_count == len == %zu\n", arr->element_count);
        fflush(stdout);
        return 0;
    }

    arr->data = mu_realloc(arr->data, arr->element_count * arr->element_size);
    arr->len = arr->element_count;

    return 0;
}
int au_is_full(u_arr *arr)
{
    if (arr == NULL)
    {
        fprintf(stderr, "au_is_full : wrong params\n");
        fflush(stderr);
        return -1;
    }
    return (arr->element_count == arr->len);
}
int au_is_empty(u_arr *arr)
{
    if (arr == NULL)
    {
        fprintf(stderr, "au_is_empty : wrong params\n");
        fflush(stderr);
        return -1;
    }
    return (arr->element_count == 0);
}
