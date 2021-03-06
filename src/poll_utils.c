#include <poll_utils.h>
#include <mem_utils.h>
#include <string.h>

u_pollarr pu_initarr(size_t maxlen)
{
	u_pollarr arr = {0};
	arr.len = 0;
	arr.maxlen = maxlen;
	arr.empty = 1;
	if (maxlen > 0)
	{
		arr.arr = (struct pollfd *)mu_calloc(maxlen * sizeof(struct pollfd));
	}
	return arr;
}
int pu_isempty(u_pollarr* arr)
{
	if (arr == NULL)
	{
		fprintf(stdout, "pu_isempty : param arr == NULL\n");
		fflush(stdout);
		return -1;
	}
	return (arr->len == 0) && arr->empty;
}
int pu_isfull(u_pollarr* arr)
{
	if (arr == NULL)
	{
		fprintf(stdout, "pu_isfull : param arr == NULL\n");
		fflush(stdout);
		return -1;
	}
	return arr->len == arr->maxlen;
}

int pu_add(u_pollarr* arr, int newfd, short events)
{
	if (arr == NULL)
	{
		fprintf(stdout, "pu_add : param arr == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (pu_isfull(arr))
	{
		fprintf(stdout, "pu_add : arr is full\n");
		fflush(stdout);
		return -1;
	}
	(arr->arr)[(arr->len)].fd = newfd;
	(arr->arr)[(arr->len)].events = events;
	(arr->arr)[(arr->len)++].revents &= 0;
	return 0;
}
int pu_remove(u_pollarr* arr, int oldfd)
{
	if (arr == NULL)
	{
		fprintf(stdout, "pu_remove : param arr == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (pu_isempty(arr))
	{
		fprintf(stdout, "pu_remove : arr is empty\n");
		fflush(stdout);
		return -1;
	}
	size_t i = arr->len > 0 ? arr->len - 1 : 0;
	for (; i > 0 && (arr->arr)[i].fd != oldfd; i--);
	if (i == 0 && (arr->arr)[i].fd != oldfd) return -1;

	//found
	if (i != (arr->len - 1))
	{
		memcpy(arr->arr + i, arr->arr + arr->len - 1,
			sizeof(struct pollfd));
	}
	memset(arr->arr + arr->len - 1, 0, sizeof(struct pollfd));
	(arr->len)--;
	return 0;

}
int pu_free(u_pollarr* arr)
{
	if (arr == NULL)
	{
		fprintf(stdout, "pu_free : param arr == NULL\n");
		fflush(stdout);
		return -1;
	}
	mu_free(arr->arr);
	arr->maxlen = 0;
	arr->len = 0;
	return 0;
}