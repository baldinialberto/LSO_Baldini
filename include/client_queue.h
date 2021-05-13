#ifndef _CLIENT_QUEUE_H
#define _CLIENT_QUEUE_H

#pragma once

#include "error_utils.h"
#include "lock_utils.h"
#include "cond_utils.h"
#include "common.h"

#define CQ_NULL_FLAG        0x2
#define CQ_INVALID_FLAG     0x4
#define CQ_NOTFOUND_FLAG    0x8
#define CQ_EMPTY_FLAG       0x10

#define CQ_DEAD_FLAG        0x0000
#define CQ_LIVE_FLAG        0x0001
#define CQ_READY_FLAG       0x0002
#define CQ_WAIT_FLAG        0x0004
#define CQ_SERVING_FLAG     0x0008


typedef struct _client_queue_node
{
    int client_socket;
    int status;
    struct _client_queue_node *next;
} client_queue_node;
typedef struct _client_queue
{
    client_queue_node *head;
    client_queue_node *tail;
    size_t nclients;
    mutex_t mutex;
    cond_t empty;
} client_queue;

/**
 * @brief Check if queue is empty
 * @param queue pointer to queue
 * @return 0 if not empty, > 0 otherwise.\
 * if queue == NULL, CQ_NULL_FLAG is set
 */
int cq_is_empty(client_queue *queue);

/**
 * @brief Push a new client_socket in queue
 * @param queue pointer to queue
 * @param client_socket new client_socket to serve
 * @return 0 if success, > 0 otherwise.         \
 * if queue == NULL, CQ_NULL_FLAG is set        \
 * if client_socket < 0, CQ_INVALID_FLAG is set \
 */
int cq_push(client_queue *queue, int client_socket);

/**
 * @brief Remove a client_socket from queue
 * @param queue pointer to queue
 * @param client_socket client_socket to remove
 * @return 0 if success, > 0 otherwise.     \
 * if queue == NULL, CQ_NULL_FLAG is set    \
 * if client_socket not in queue,           \
 *  CQ_NOTFOUND_FLAG is set                 \
 */
int cq_remove(client_queue *queue, int client_socket);

/**
 * @brief Deprecated
 */
__attribute_deprecated__ int cq_serve(client_queue *queue);

#endif