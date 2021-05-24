#ifndef _CLIENT_H
#define _CLIENT_H

#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "error_utils.h"
#include "serverapi.h"

#define STRING_STDLEN 256

typedef struct _client_conf
{
    char *server_socket_filename;
    char *writeback_foldername;
    char *folder_to_write;
    char *folder_destination;
    ln_ptr files_to_write;
    ln_ptr files_to_read;
    ln_ptr files_to_lock;
    ln_ptr files_to_unlock;
    ln_ptr files_to_delete;
    unsigned int folder_filecount;
    unsigned int read_filecount;
    unsigned int connection_timer;
    bool verbose;
} client_conf;

int parseargs(int argc, char ** argv, client_conf *conf);
int printargs(client_conf *conf);
int client_conf_cleanup(client_conf *conf);
int conf_add_list(const char *optarg, ln_ptr* list);
int write_files(ln_ptr list, const char *writeback_folder);
int write_folder(char *folderpath, int count);
int read_files(ln_ptr list, const char *folder_dest);
int read_rndm_nfiles(int count);
int remove_files(ln_ptr list);
int write_file(ln_ptr list_node, const char *writeback_folder);
int read_file(ln_ptr list_node, const char *folder_dest);


#endif