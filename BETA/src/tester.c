#include "../include/list_utils.h"
#include "../include/string_utils.h"
#include "../include/hash_utils.h"

int main(void)
{
    u_list list = lu_empty_list();
    for (int i = 0; i < 100; i++)
    {
        lu_insert_oncopy(&list, &i, sizeof(int), lu_int_compare);
    }
    for (int i = 0; i < 100; i+=2)
    {
        lu_remove(&list, &i, lu_int_compare, free);
    }
    lu_print(list, lu_int_print);

    int *arr = (int *)lu_plainarr(list, sizeof(int));

    for (int i = 0; i < lu_length(list); i++)
    {
        printf("%d\n", arr[i]);
    }

    u_list stringlist = lu_empty_list();
    for (char c = (char)1; c <= (char)120; c++)
    {
        lu_insert_oncopy(&stringlist, &c, sizeof(char), lu_char_compare);
    }
    char *string = lu_tostring(stringlist);
    lu_free(&stringlist, free);


    printf("string = %s\n", string);

    free(string);
    free(arr);
    lu_free(&list, free);

    return 0;
}