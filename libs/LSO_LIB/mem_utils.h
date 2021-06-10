//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_MEM_UTILS_H
#define LSO_LIB_MEM_UTILS_H

#pragma once
#include <stdio.h>

void *mu_malloc(unsigned long int size);
void *mu_realloc(void *ptr, unsigned long int size);
void *mu_calloc(unsigned long int size);
void mu_free(void *ptr);
void *mu_clone(const void *ptr, size_t size);

#endif //LSO_LIB_MEM_UTILS_H
