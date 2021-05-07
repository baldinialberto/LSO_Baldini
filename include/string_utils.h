#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct _string
{
	char* data;
	size_t len;
} string;

string create_string(size_t len);
string create_string_from_literal(const char* str);
string string_cat(string head, string tail);
string string_append(string oldString, const char *append);
void free_string(string* str);
string* realloc_string(string* str, size_t newSize);

#endif