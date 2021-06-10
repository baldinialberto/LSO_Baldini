#include "mem_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *mu_malloc(unsigned long int size) {
    if (size) {
        void *res = malloc(size);
        if (res == NULL) {
            perror("mu_malloc");
            exit(-1);
        }
        return res;
    }
    return NULL;
}

void *mu_realloc(void *ptr, unsigned long int size) {
    if (size) {
        void *res = realloc(ptr, size);
        if (res == NULL) {
            perror("mu_realloc");
            exit(-1);
        }
        return res;
    }
    return NULL;
}

void *mu_calloc(unsigned long int size) {
    if (size) {
        void *res = calloc(1, size);
        if (res == NULL) {
            perror("mu_calloc");
            exit(-1);
        }
        return res;
    }
    return NULL;
}

void mu_free(void *ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void *mu_clone(void *ptr, size_t size)
{
    if (ptr == NULL || size == 0) return NULL;
    void *res = mu_malloc(size);
    memcpy(res, ptr, size);
    return res;
}
