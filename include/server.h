#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "server_message.h"
#include "poll_utils.h"
#include "../libs/LSO_LIB/file_utils.h"

#define CONFIG_NWORKERS "NWORKERS"
#define CONFIG_CAPACITY "CAPACITY"
#define CONFIG_FILECAPACITY "FILECAPACITY"
#define CONFIG_SOCKET_NAME    "SOCKET_NAME"
#define CONFIG_LOGFILE_NAME "LOGFILE_NAME"
#define CONFIG_CLIENT_MAX "CLIENT_MAX"
#define SERVER_CONFIGFILE_PATH "./config.txt"

#define SAPI_RESPOND(MESSAGE, CLIENT)                    \
if (server_respond(MESSAGE, CLIENT))                        \
{                                                        \
    fprintf(stdout, "server_respond at %s\n", __func__);    \
    fflush(stdout);                                    \
    return -1;                                            \
}

#define CHECK_READ_RESPONSE(client, dest, size, message) \
if (read(client, &(dest), size) < size)                        \
{                                                            \
    if (errno)                                                \
    {                                                        \
        perror(message);                                    \
        errno = 0;                                            \
    }                                                    \
    if (server_respond(SAPI_FAILURE, client))                \
    {                                                        \
        fprintf(stdout, "read at %s\n", __func__);            \
        fflush(stdout);                                        \
        return SAPI_FAILURE;                                \
    }                                                        \
    return SAPI_FAILURE;                                    \
}

#define DEBUG(X) X

typedef struct server_settings {
	unsigned short n_workers;
	unsigned int available_Memory;
	unsigned int maxFileCount;
	char socket_name[64];
	char logfile_name[64];
	unsigned int maxClientCount;
} server_settings;

typedef struct server_stats {
	unsigned int nfile_max;            // max number of files in server at once
	unsigned int MB_max;            // max memory load in server
	unsigned int nfile;                // number of files in server at exit
	unsigned int MB;                // server capacity
	unsigned int ncache_replace;    // number of cache replaces
} server_stats;

typedef struct server_infos {
	u_file_storage storage;
	int server_socket_fd;            // file descriptor of sever_socket
	unsigned short n_workers;        // number of workers of the server
	pthread_t* workers;                // array of workers
	int* workers_clients;            // number of clients served by each worker
	int server_quit;
	int server_hu;
	u_pollarr poll_arr;
	pthread_cond_t* worker_conds;
	pthread_mutex_t* worker_locks;
	pthread_mutex_t dispatcher_lock;
	pthread_cond_t workers_busy;
} server_infos;

struct worker_arg {
	int worker_id;
	server_infos* infos;
};

int init_server_settings(server_settings* setts);
int init_server_infos(server_settings* setts, server_infos* infos);
int free_server_infos(server_infos* infos);
void print_server_settings(server_settings* setts);
int write_log(const char* op, server_settings* setts);
void get_setting(char** str, FILE* fstream, server_settings* setts);
int create_server_socket(server_settings* setts);
int ignore_signals();
int spawn_workers(server_infos* infos);
int join_workers(server_infos* infos);
void* server_signal_handler(void* infos);
void server_dispatcher(server_infos* infos);
void* server_worker(void* worker_args);
int assign_client(server_infos* infos, int client);
int serve(s_message request, int client_socket, u_file_storage* storage);
void server_skipline(FILE* fstream);
int server_readline(FILE* fstream, char** str);
int server_readword(FILE* fstream, char** str);
int server_openFile(s_message message, int client, u_file_storage* storage);
int server_closeFile(s_message message, int client, u_file_storage* storage);
int server_readFile(s_message message, int client, u_file_storage* storage);
int server_readNFiles(s_message message, int client, u_file_storage* storage);
int server_writeFile(s_message message, int client, u_file_storage* storage);
int server_appendToFile(s_message message, int client, u_file_storage* storage);
int server_lockFile(s_message message, int client, u_file_storage* storage);
int server_unlockFile(s_message message, int client, u_file_storage* storage);
int server_removeFile(s_message message, int client, u_file_storage* storage);
int server_evict(int client, u_file_storage *storage, size_t bytes_to_free);
char* server_get_path(s_message message, int client);
int server_send_path(int client, const char *path);
size_t server_get_len(int client);
int server_respond(s_message message, int client);
int server_send_data(int client, void* data, size_t data_len);

#endif