#ifndef _SERVER_UTILS_H
#define _SERVER_UTILS_H

#pragma once
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "error_utils.h"
#include "common.h"
#include "commonio.h"
#include "string_utils.h"

#define CONFIG_NWORKERS "NWORKERS"
#define CONFIG_CAPACITY "CAPACITY"
#define CONFIG_SOCKET_NAME	"SOCKET_NAME"
#define CONFIG_LOGFILE_NAME "LOGFILE_NAME"

#define SERVER_CONFIGFILE_PATH "config.txt"

typedef struct _server_settings
{
	//char *logfile_path;
	unsigned short nworkers;
	unsigned int MB_dim;
	char socket_name[64];
	char logfile_name[64];
} server_settings;

typedef struct _server_stats
{
	unsigned int nfile_max;
	unsigned int MB_max;
	unsigned int nfile;
	unsigned int MB;
	unsigned int ncache_replace;
} server_stats;


void print_server_settings(server_settings *setts);
server_settings parse_settings();
int write_settings(server_settings* settings);
int write_log(const char* op, server_settings* settings);
void get_setting(char** str, FILE *fstream, server_settings *settings);

#endif