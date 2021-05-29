#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <pthread.h>

#include "server_message.h"
#include "error_utils.h"
#include "string_utils.h"

#define UNIX_PATH_MAX 108

#define CONFIG_NWORKERS "NWORKERS"
#define CONFIG_CAPACITY "CAPACITY"
#define CONFIG_FILECAPACITY "FILECAPACITY"
#define CONFIG_SOCKET_NAME	"SOCKET_NAME"
#define CONFIG_LOGFILE_NAME "LOGFILE_NAME"

typedef struct _server_settings
{
	unsigned short nworkers;		// 
	unsigned int avaiableMemory;
	unsigned int maxFileCount;
	char socket_name[64];
	char logfile_name[64];
} server_settings;

typedef struct _server_stats
{
	unsigned int nfile_max;			// max number of files in server at once
	unsigned int MB_max;			// max memory load in server
	unsigned int nfile;				// number of files in server at exit
	unsigned int MB;				// server capacity
	unsigned int ncache_replace;	// number of chache replaces
} server_stats;

typedef struct _server_infos
{
	SFS_FS *memory;
	int server_socket_fd;		// file descriptor of sever_socket
	unsigned short nworkers;	// number of workers of the server
	pthread_t *workers;			// array of workers
	int *workers_clients;		// number of clients served by each worker
	int server_quit;
	int server_hu;
	socket_queue *sq;
	cond_t *worker_conds;
	mutex_t *worker_locks;
} server_infos;

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