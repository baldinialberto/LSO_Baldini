#ifndef _MEM_UTILS_H
#define _MEM_UTILS_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *mu_malloc(size_t size);
void *mu_realloc(void *ptr, size_t size);
void *mu_calloc(size_t size);
void mu_free(void *ptr);

#endif