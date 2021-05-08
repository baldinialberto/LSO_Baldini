#include <error_utils.h>
#include <string_utils.h>

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

int string_cat(string* head, const string* tail)
{
	return 0;
}

int string_append(string *oldString, const char *append)
{
	if (oldString == NULL || append == NULL)
	{
		fprintf(stderr, "string_append : arg is NULL\n");
		return -1;
	}
	size_t appendLen = strlen(append), oldStringLen = oldString->len;
	CHECK_BADVAL_PERROR_RETURN(
		realloc_string(oldString, oldString->len + appendLen),
		NULL, "realloc_string", -1
	);
	memcpy(oldString + oldStringLen, append, appendLen);
	return 0;
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
		return (string *)-1;

	memset(temp + str->len, (char)0, 
		(newSize - str->len) * sizeof(char)
	);
	str->data = temp;
	str->len = newSize - 1;

	return str;
}
