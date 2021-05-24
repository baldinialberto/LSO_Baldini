#include "../include/mem_utils.h"


void *mu_malloc(size_t size)
{
    void *res = malloc(size);
    if (res == NULL)
        exit(-1);
    return res;
}
void *mu_realloc(void *ptr, size_t size)
{
    if ((ptr = realloc(ptr, size)) == NULL)
        exit(-1);
    return ptr;
}
void *mu_calloc(size_t size)
{
    void *res = calloc(size, 1);
    if (res == NULL)
        exit(-1);
    return res;
}
void mu_free(void *ptr)
{
    if (ptr != NULL)
        free(ptr);
}