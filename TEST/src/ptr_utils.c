#include <ptr_utils.h>

void *pu_voidptr_increase(void *base_ptr, size_t bytes)
{
    return (void *)((char*)base_ptr + bytes);
}

void *pu_voidptr_decrease(void *base_ptr, size_t bytes)
{
    return (void *)((char *)base_ptr - bytes);
}
