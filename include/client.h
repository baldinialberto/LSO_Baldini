#ifndef _CLIENT_H
#define _CLIENT_H

#pragma once

#include "serverapi.h"
#include "common.h"

#define STRING_STDLEN 256

typedef struct _strnode
{
    char *data;
    struct _sttnode *next;
} strnode;
typedef strnode *strnode_ptr;

typedef struct _client_conf
{
    char server_socket_filename[STRING_STDLEN];
    char writeback_foldername[STRING_STDLEN];
    char folder_to_write[STRING_STDLEN];
    char folder_destination[STRING_STDLEN];
    strnode_ptr files_to_write;
    strnode_ptr files_to_read;
    strnode_ptr files_to_lock;
    strnode_ptr files_to_unlock;
    strnode_ptr files_to_delete;
    unsigned int folder_filecount;
    unsigned int read_filecount;
    unsigned int connection_timer;
    bool verbose;
} client_conf;

#endif