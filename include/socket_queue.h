#ifndef _SOCKET_QUEUE_H
#define _SOCKET_QUEUE_H

#pragma once

#include <sys/poll.h>

#include "error_utils.h"
#include "lock_utils.h"
#include "cond_utils.h"
#include "common.h"

#define SQ_CSOCKET_FLAG     0x1
#define SQ_NULL_FLAG        0x2
#define SQ_INVALID_FLAG     0x4
#define SQ_NOTFOUND_FLAG    0x8
#define SQ_EMPTY_FLAG       0x10
#define SQ_SSOCKET_FLAG     0x20

#define SQ_DEAD_FLAG        0x0000
#define SQ_LIVE_FLAG        0x0001
#define SQ_READY_FLAG       0x0002
#define SQ_WAIT_FLAG        0x0004
#define SQ_SERVING_FLAG     0x0008


typedef struct _socket_queue_node
{
    int socket_fd;
    int status;
    //mutex_t mutex;
    struct _socket_queue_node *next;
} socket_queue_node;
typedef struct _socket_queue
{
    socket_queue_node *head;
    socket_queue_node *tail;
    size_t nsockets;
    size_t nclients;
    mutex_t mutex;
    cond_t full;
    struct pollfd *pollarr;
} socket_queue;

/**
 * @brief Check if queue is empty
 * @param queue pointer to queue
 * @return 0 if not empty, > 0 otherwise.\
 * if queue == NULL, SQ_NULL_FLAG is set
 */
int sq_is_empty(socket_queue *queue);

/**
 * @brief Check if queue has clients
 * @param queue pointer to queue
 * @return 0 if has not, > 0 otherwise.\
 * if queue == NULL, SQ_NULL_FLAG is set
 */
int sq_has_clients(socket_queue *queue);

/**
 * @brief Push a new client_socket in queue
 * @param queue pointer to queue
 * @param client_socket new client_socket to serve
 * @param flag indicates client (CQ_CSOCKET) or server (CQ_SSOCKET)
 * @return 0 if success, > 0 otherwise.         \
 * if queue == NULL, SQ_NULL_FLAG is set        \
 * if client_socket < 0, SQ_INVALID_FLAG is set \
 */
int sq_push(socket_queue *queue, int client_socket, int flag);

/**
 * @brief Remove a client_socket from queue
 * @param queue pointer to queue
 * @param client_socket client_socket to remove
 * @return 0 if success, > 0 otherwise.     \
 * if queue == NULL, SQ_NULL_FLAG is set    \
 * if client_socket not in queue,           \
 *  SQ_NOTFOUND_FLAG is set                 \
 */
int sq_remove(socket_queue *queue, int client_socket);

/**
 * @brief update arr of clients
 * @param  queue pointer to queue
 * @return 0 if successfull, > 0 otherwise  \
 * if queue->sockarr_valid = 1 return SQ_INVALID_FLAG
 * if queue == NULL return SQ_NULL_FLAG
 */
int sq_update_arr(socket_queue *queue);

/**
 * @brief free queue of clients
 * @param queue pointer to queue
 * @return 0 if sucessfull, > 0 otherwise
 * if queue == NULL return SQ_NULL_FLAG
 */
int sq_free(socket_queue *queue);

int sq_print(socket_queue *queue);

#endif