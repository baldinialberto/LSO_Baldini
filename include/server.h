#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <file_utils.h>
#include <list_utils.h>
#include "poll_utils.h"
#include "server_message.h"
#define UNIX_PATH_MAX 108

#define CONFIG_NWORKERS "NWORKERS"
#define CONFIG_CAPACITY "CAPACITY"
#define CONFIG_FILECAPACITY "FILECAPACITY"
#define CONFIG_SOCKET_NAME	"SOCKET_NAME"
#define CONFIG_LOGFILE_NAME "LOGFILE_NAME"
#define CONFIG_CLIENT_MAX "CLIENT_MAX"
#define SERVER_CONFIGFILE_PATH "./config.txt"

#define DEBUG(X) X

typedef struct server_settings
{
	unsigned short nworkers;		// 
	unsigned int avaiableMemory;
	unsigned int maxFileCount;
	char socket_name[64];
	char logfile_name[64];
	unsigned int maxClientCount;
} server_settings;

typedef struct server_stats
{
	unsigned int nfile_max;			// max number of files in server at once
	unsigned int MB_max;			// max memory load in server
	unsigned int nfile;				// number of files in server at exit
	unsigned int MB;				// server capacity
	unsigned int ncache_replace;	// number of chache replaces
} server_stats;

typedef struct server_infos
{
	u_file_storage storage;
	int server_socket_fd;		// file descriptor of sever_socket
	unsigned short nworkers;	// number of workers of the server
	pthread_t *workers;			// array of workers
	int *workers_clients;		// number of clients served by each worker
	int server_quit;
	int server_hu;
	u_pollarr pollarr;
	pthread_cond_t *worker_conds;
	pthread_mutex_t *worker_locks;
} server_infos;

struct worker_arg
{
    int worker_id;
    server_infos *infos;
};

int init_server_settings(server_settings *setts);
int init_server_infos(server_settings *setts, server_infos* infos);
int free_server_infos(server_infos *infos);
void print_server_settings(server_settings *setts);
server_settings parse_settings();
int write_log(const char* op, server_settings* setts);
void get_setting(char** str, FILE *fstream, server_settings *setts);
int create_server_socket(server_settings *setts);
int ignore_signals();
int spawn_workers(server_infos* infos);
int join_workers(server_infos* infos);
void *server_signal_handler(void *infos);
void server_dispatcher(server_infos *infos);
void *server_worker(void *worker_args);
int assign_client(server_infos *infos, int client);
int serve(int request, int client_socket, u_file_storage* storage);
void server_skipline(FILE *fstream);
int server_readline(FILE *fstream, char **str);
int server_readword(FILE *fstream, char **str);
#endif