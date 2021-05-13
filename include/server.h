#ifndef _SERVER_H
#define _SERVER_H

#pragma once

#include <sys/signal.h>

#include "server_utils.h"
#include "server_common.h"
#include "client_queue.h"

void *server_signalhandler(void *infos);
void *server_dispatcher(void *infos);
void *server_worker(void *clients);

#endif