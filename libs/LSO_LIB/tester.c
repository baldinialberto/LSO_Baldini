//
// Created by Alberto Baldini on 04/06/21.
//

#include "hash_utils.h"
#include "mem_utils.h"
#include <string.h>
#include <stdio.h>

int string_compare(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}
void int_print(const void *a)
{
    printf("%s = %d\n", ((char *)(((u_hash_item *)a)->key)), *((int *)(((u_hash_item *)a)->data)));
}

int main(void)
{
    u_hash_table table = hu_init(10, hu_hash_string, string_compare, int_print, mu_free, mu_free);
    char temp[10];
    printf("hash table is%s empty\n", hu_is_empty(&table) ? "" : " not");
    for (int i = 0; i < 400; i++)
    {
        snprintf(temp, 9, "%d", i);
        //printf("insert %s\n", temp);
        hu_insert(&table, mu_clone((void *)temp,strlen(temp) + 1), mu_clone(((void *)&i), sizeof(int)));
    }
    for (int i = 0; i < 400; i+=2)
    {
        snprintf(temp, 9, "%d", i);
        //printf("remove %s\n", temp);
        hu_remove(&table, temp);
    }
    hu_print(&table);
    hu_free(&table);

    return 0;
}
