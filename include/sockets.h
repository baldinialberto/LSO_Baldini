#ifndef _SOCKETS_H
#define _SOCKETS_H

#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <string.h>

#include "error_utils.h"

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#define UNIX_PATH_MAX 108

void socket_errno_check(const char *source);
int create_server_socket(const char* socketName, int queueLen);
int create_client_socket();
int connect_to_server_socket(const char* socketName);
int socket_write(int sock_fd, void* buff, size_t len);
int socket_read(int sock_fd, void* buff, size_t size);

#endif
