//
// Created by Alberto Baldini on 04/06/21.
//

#include "array_utils.h"
#include "list_utils.h"
#include "mem_utils.h"
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


    return 0;
}
