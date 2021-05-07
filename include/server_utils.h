#ifndef _SERVER_UTILS_H
#define _SERVER_UTILS_H

#pragma once
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define SERVER_CONFIGFILE_PATH "serverConfig.text"

typedef struct _server_settings
{
	const char *logfile_path;
	const unsigned short nworkers;
	const unsigned int MB_dim;
	const char* socket_name;
} server_settings;

typedef struct _server_stats
{
	unsigned int nfile_max;
	unsigned int MB_max;
	unsigned int nfile;
	unsigned int MB;
	unsigned int ncache_replace;
} server_stats;

server_settings parse_settings();
int write_settings(server_settings* settings);
int write_log(const char* op, server_settings* settings);

#endif