#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <sys/signal.h>

#include "server_utils.h"
#include "server_common.h"
#include "client_queue.h"

int ignore_signals();
int spawn_workers(server_infos* infos);
int join_workers(server_infos* infos);
void *server_signalhandler(void *infos);
void server_dispatcher(server_infos *infos);
void *server_worker(void *client);
int assign_client(server_infos *infos, int client);

int server_closeConnection(int cliend_socket);
int server_openFile(int cliend_socket);
int server_readFile(int cliend_socket);
int server_writeFile(int cliend_socket);
int server_appendToFile(int cliend_socket);
int server_lockFile(int cliend_socket);
int server_unlockFile(int cliend_socket);
int server_closeFile(int cliend_socket);
int server_removeFile(int cliend_socket);

#endif