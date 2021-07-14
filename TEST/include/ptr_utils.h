#ifndef PTR_UTILS_H
#define PTR_UTILS_H

#include <sys/types.h>

/**
 * Void pointer increase
 * @param base_ptr address of ptr
 * @param bytes bytes to shift
 * @return new address of ptr increased of @param bytes
 */
void *pu_voidptr_increase(void *base_ptr, size_t bytes);

/**
 * Void pointer decrease
 * @param base_ptr address of ptr
 * @param bytes bytes to shift
 * @return new address of ptr decreased of @param bytes
 */
void *pu_voidptr_decrease(void *base_ptr, size_t bytes);

#endif
