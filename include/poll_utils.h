
#ifndef _POLL_UTILS_H
#define _POLL_UTILS_H

#pragma once

#include <sys/poll.h>
#include <stdlib.h>

typedef struct _pollarr
{
    struct pollfd *arr;
    size_t len;
    size_t maxlen;
} u_pollarr;

u_pollarr pu_initarr(size_t maxlen);
int pu_isempty(u_pollarr *arr);
int pu_isfull(u_pollarr *arr);
size_t pu_length(u_pollarr *arr);
struct pollfd *pu_arr(u_pollarr *arr);
int pu_add(u_pollarr *arr, int newfd, short events);
int pu_remove(u_pollarr *arr, int oldfd);
int pu_free(u_pollarr *arr);

#endif