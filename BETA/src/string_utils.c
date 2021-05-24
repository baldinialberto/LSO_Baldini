#include "../include/string_utils.h"

u_string su_string_from_literal(const char *literal)
{
    if (literal == NULL)
    {
        fprintf(stdout, "su_string_from_literal : param literal == NULL\n");
        fflush(stdout);
        return (u_string){NULL, 0};
    }
    size_t literal_len = strlen(literal);
    u_string new_string;
    new_string.data = mu_malloc(literal_len + 1);
    new_string.len = literal_len;
    return new_string;
}
int su_free_string(u_string *string)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_free_string : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    mu_free(string->data);
    return 0;
}
int su_append_literal(u_string *string, const char *literal)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_append_literal : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    if (literal == NULL)
    {
        fprintf(stdout, "su_append_literal : param literal == NULL\n");
        fflush(stdout);
        return -1;
    }
    size_t literal_len = strlen(literal), i = string->len;
    if (su_string_extend(string, literal_len)) return -1;
    strcpy(string->data + i, literal);

    return 0;
}
int su_string_extend(u_string *string, size_t delta)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_string_extend : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    if (delta == 0) return 0;

    return su_realloc(string, string->len + delta);
}
int su_string_shrinktofit(u_string *string)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_string_shrinktofit : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    return su_realloc(string, su_strlen(string));
}
int su_realloc(u_string *string, size_t newsize)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_realloc : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    string->len = newsize;
    string->data = mu_realloc(string->data, newsize + 1);

    return 0;
}

int su_strlen(u_string *string)
{
    if (string == NULL)
    {
        fprintf(stdout, "su_strlen : param string == NULL\n");
        fflush(stdout);
        return -1;
    }
    return strlen(string->data);
}