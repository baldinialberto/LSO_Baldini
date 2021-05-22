#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <sys/signal.h>

#include "server_utils.h"
#include "socket_queue.h"

#define S_NOREAD        0x4
#define S_NOWRITE       0x5

struct _worker_arg
{
    int worker_id;
    server_infos *infos;
};

int ignore_signals();
int spawn_workers(server_infos* infos);
int join_workers(server_infos* infos);
void *server_signalhandler(void *infos);
void server_dispatcher(server_infos *infos);
void *server_worker(void *worker_args);
int assign_client(server_infos *infos, int client);
int serve(int request, int client_socket, SFS_FS* memory);

int server_closeConnection(int request, server_infos* infos);
int server_openFile(int request, int client_socket, SFS_FS* memory);
int server_readFile(int request, int client_socket, SFS_FS* memory);
int server_writeFile(int request, int client_socket, SFS_FS* memory);
int server_appendToFile(int request, int client_socket, SFS_FS* memory);
int server_lockFile(int request, int client_socket, SFS_FS* memory);
int server_unlockFile(int request, int client_socket, SFS_FS* memory);
int server_closeFile(int request, int client_socket, SFS_FS* memory);
int server_removeFile(int request, int client_socket, SFS_FS* memory);

#endif