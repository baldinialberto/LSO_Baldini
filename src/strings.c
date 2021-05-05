#include "../include/strings.h"
#include "../include/errors.h"

string create_string(size_t len)
{
	string res;

	res.data = calloc(len + 1, sizeof(char));
	res.len = res.data == NULL ? 0 : len;

	return res;
}

string create_string_from_literal(const char* str)
{
	string res;
	res.len = strlen(str);
	res.data = calloc(res.len + 1, sizeof(char));
	memcpy(res.data, str, res.len * sizeof(char));
	return res;
}

void free_string(string* str)
{
	free(str->data);
	str->data = NULL;
	str->len = 0;
}

string* realloc_string(string* str, size_t newSize)
{
	newSize++;
	char* temp = realloc(str->data, newSize);
	if (temp == NULL)
		return -1;

	memset(temp + str->len, (char)0, 
		(newSize - str->len) * sizeof(char)
	);
	str->data = temp;
	str->len = newSize - 1;

	return str;
}
