#include "../include/list_utils.h"

u_list lu_empty_list()
{
    return NULL;
}
int lu_insert(u_list *list, void *data, int (*datacompare)(void *, void *))
{
    if (list == NULL)
    {

    }
    if (data == NULL)
    {
        
    }
}
int lu_search(u_list *list, int (*datasearch)(void *))
{

}
int lu_length(u_list list)
{

}
int lu_free(u_list *list, void (*datafree)(void *))
{

}
int lu_print(u_list list, void (*dataprint)(void *))
{

}
char *lu_plainarr(u_list list, char* (dataextract)(void *))
{

}