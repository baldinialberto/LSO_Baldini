//
// Created by Alberto Baldini on 04/06/21.
//

#include "array_utils.h"
#include <stdio.h>

void print(const void *ptr)
{
    printf("%d\n", *((int *)ptr));
}
int int_comp(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}

int main(void)
{
    u_arr array = au_init_arr(100, sizeof(int), int_comp, print, NULL);
    int i = 100;
    for (; i > 0; i--)
    {
        au_insert(&array, (const void *)&(i));
    }
    for (i = 1; i < 100; i+= 2)
    {
        au_remove(&array, (const void *)&(i));
    }
    au_shrinks(&array);
    au_sort(&array);
    au_print(&array);
    au_extends(&array, 200);
    i = 200;
    for (; i > 0 && !au_is_full(&array); i--)
    {
        au_insert(&array, (const void *)&(i));
    }
    au_sort(&array);
    au_print(&array);
    au_extends(&array, 1);
    au_print(&array);
    au_free(&array);

    return 0;
}
