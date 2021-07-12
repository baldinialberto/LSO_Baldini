//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_STRING_UTILS_H
#define LSO_LIB_STRING_UTILS_H

#pragma once
#include <stdlib.h>

typedef struct utils_string
{
	char* data;
	size_t len;
} u_string;

u_string su_string_from_literal(const char* literal);
int su_free_string(u_string* string);
int su_append_chars(u_string* string, const char* literal);
int su_realloc(u_string* string, size_t newsize);
size_t su_strlen(u_string* string);
int su_string_extend(u_string* sting, size_t delta);
int su_string_shrinktofit(u_string* string);
int su_u_string_compare(const void* a, const void* b);
int su_string_compare(const void* a, const void* b);
void su_string_print(const void* s);

#endif //LSO_LIB_STRING_UTILS_H
