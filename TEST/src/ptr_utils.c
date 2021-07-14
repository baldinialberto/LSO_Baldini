#include <ptr_utils.h>
#include <assert.h>

void *pu_voidptr_increase(void *base_ptr, size_t bytes)
{
    assert(sizeof(void*)==sizeof(size_t));
    return (void *)((size_t)base_ptr + bytes);
}

void *pu_voidptr_decrease(void *base_ptr, size_t bytes)
{
    assert(sizeof(void*)==sizeof(size_t));
    return bytes > (size_t)base_ptr ? (void*)0 : (void *)((size_t)base_ptr - bytes);
}
