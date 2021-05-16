#ifndef _SERVER_UTILS_H
#define _SERVER_UTILS_H

#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <pthread.h>

#include "server_common.h"
#include "server_fs.h"
#include "error_utils.h"
#include "common.h"
#include "commonio.h"
#include "string_utils.h"
#include "lock_utils.h"
#include "cond_utils.h"

#define UNIX_PATH_MAX 108

#define CONFIG_NWORKERS "NWORKERS"
#define CONFIG_CAPACITY "CAPACITY"
#define CONFIG_FILECAPACITY "FILECAPACITY"
#define CONFIG_SOCKET_NAME	"SOCKET_NAME"
#define CONFIG_LOGFILE_NAME "LOGFILE_NAME"

#define SERVER_CONFIGFILE_PATH "config.txt"

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
	cond_t *worker_conds;
	mutex_t *worker_locks;
} server_infos;

server_settings init_server_settings();
server_stats init_server_stats();
server_infos init_server_infos(server_settings *setts);
int free_server_infos(server_infos *infos);
void print_server_settings(server_settings *setts);
server_settings parse_settings();
int write_settings(server_settings* setts);
int write_log(const char* op, server_settings* setts);
void get_setting(char** str, FILE *fstream, server_settings *setts);
int create_server_socket(server_settings *setts);
int thread_spawn_detached(void *(*fun)(void*), void *arg);
pthread_t thread_spawn(void *(*fun)(void*), void *arg);


#endif