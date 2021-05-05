#ifndef _STRINGS_H
#define _STRINGS_H

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
void free_string(string* str);
string* realloc_string(string* str, size_t newSize);

#endif