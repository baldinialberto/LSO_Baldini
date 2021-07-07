#ifndef _SERVERAPI_H
#define _SERVERAPI_H

#pragma once

extern int server_socket_fd;

#include "server_message.h"
#include <stdio.h>
#include <time.h>

int sapi_evict(const char *dirname);
void sapi_printerror(FILE *fstream, s_message message);
int sapi_sendop(unsigned int messlen, unsigned char op, unsigned char flag);
int sapi_senddatalen(size_t datalen);
int sapi_senddata(void *data, size_t datalen);
s_message sapi_getresponse();
int sapi_getdata(void **data, size_t *datalen);
int openConnection(const char* sockname, int msec, struct timespec abstime);
int closeConnection(const char* sockname);
int openFile(const char* pathname, int flags);
int readFile(const char* pathname, void** buf, size_t* size);
int readNFiles(int N, const char *dirname);
int writeFile(const char* pathname, const char* dirname);
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname);
int lockFile(const char* pathname);
int unlockFile(const char* pathname);
int closeFile(const char* pathname);
int removeFile(const char* pathname);

#endif