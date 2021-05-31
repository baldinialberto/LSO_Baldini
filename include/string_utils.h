#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem_utils.h"

typedef struct _utils_string
{
    char *data;
    size_t len;
} u_string;

u_string su_string_from_literal(const char *literal);
int su_free_string(u_string *string);
int su_append_chars(u_string *string, const char *literal);
int su_realloc(u_string *string, size_t newsize);
int su_strlen(u_string *string);
int su_string_extend(u_string *sting, size_t delta);
int su_string_shrinktofit(u_string *string);


#endif