#ifndef _CLIENT_H
#define _CLIENT_H

#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "mem_utils.h"
#include "list_utils.h"
#include "dir_utils.h"
#include "string_utils.h"
#include "serverapi.h"
#include "server_message.h"

typedef struct _client_conf
{
    char *server_socket_filename;
    char *writeback_foldername;
    char *folder_to_write;
    char *folder_destination;
    u_list files_to_write;
    u_list files_to_read;
    u_list files_to_lock;
    u_list files_to_unlock;
    u_list files_to_delete;
    unsigned int folder_filecount;
    unsigned int read_filecount;
    unsigned int connection_timer;
    char verbose;
} client_conf;

int parseargs(int argc, char ** argv, client_conf *conf);
int printargs(client_conf *conf);
int client_conf_cleanup(client_conf *conf);
int conf_add_list(const char *optarg, u_list* list);
int write_nfiles_from_dir(const char *dirname, int nfiles, const char *wbdir);
int write_files_list(u_list filelist, const char *wbdir);
int read_files_list(u_list filelist, const char *destdir);
int lock_files_list(u_list filelist);
int unlock_files_list(u_list filelist);
int remove_files_list(u_list filelist);

#endif