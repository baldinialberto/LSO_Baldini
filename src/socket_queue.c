#include <socket_queue.h>

int sq_is_empty(socket_queue *queue)
{
    if (queue == NULL) return SQ_NULL_FLAG;

    return queue->nsockets == 0;
}

int sq_has_clients(socket_queue *queue)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    
    return queue->nclients != 0;
}

int sq_push(socket_queue *queue, int client_socket, int flag)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    if (client_socket < 0) return SQ_INVALID_FLAG;

DEBUG(puts("----sq_push----"));

    socket_queue_node* newclient = calloc(1, sizeof(socket_queue_node));
    newclient->socket_fd = client_socket;
    newclient->status = SQ_LIVE_FLAG | flag;

    if (sq_is_empty(queue))
    {
        queue->head = newclient;
        queue->tail = newclient;
    } else {
        queue->tail->next = newclient;
        newclient->next = NULL;
        queue->tail = newclient;
    }

    (queue->nsockets)++;
    if (newclient->status & SQ_CSOCKET_FLAG) (queue->nclients)++;
    queue->pollarr_valid = 0;
    
DEBUG(printf("nsockets = %ld, nclients = %ld\n", queue->nsockets, queue->nclients));

    return 0;
}
int sq_remove(socket_queue *queue, int client_socket)
{

DEBUG(puts("----sq_remove----"));

    if (queue == NULL) return SQ_NULL_FLAG;
    if (client_socket < 0) return SQ_INVALID_FLAG;
    if (sq_is_empty(queue)) return SQ_EMPTY_FLAG;

    socket_queue_node *curr = queue->head, *prev = NULL;
    
    while (curr != NULL && curr->socket_fd != client_socket) // linear scan
    {
        prev = curr;
        curr = curr->next;
    }
    if (curr->socket_fd == client_socket) // found
    {
        if (curr->status & SQ_CSOCKET_FLAG) queue->nclients--;
        queue->nsockets--;
        queue->pollarr_valid = 0;

        if (prev == NULL) // remove head
        {
            queue->head = curr->next;
            if (curr == queue->tail) queue->tail = NULL;
            free(curr);
        }
        else if (curr == queue->tail) // remove tail
        {
            queue->tail = prev;
            queue->tail->next = NULL;
            free(curr);
        } else 
        {
            prev->next = curr->next;
            free(curr);
        }

        return 0;
    }

    return SQ_NOTFOUND_FLAG;
}

int sq_update_arr(socket_queue *queue)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    if (queue->pollarr_valid) return SQ_INVALID_FLAG;

    CHECK_BADVAL_PERROR_EXIT(
        queue->pollarr = realloc(queue->pollarr, 
            queue->nsockets * sizeof(struct pollfd)), 
        NULL, "sq_update_arr : realloc"
    )
    int i = 0;
    for (socket_queue_node* node = queue->head; node != NULL; node = node->next)
    {
       (queue->pollarr)[i].fd = node->socket_fd;
       (queue->pollarr)[i++].events = POLLIN | POLLHUP;
    }
    queue->pollarr_valid = 1;
    
    if (i != queue->nsockets)
    {
        printf("sq_update_arr : i = %d, queue->nsockets = %ld, queue->nclients = %ld\n", i, queue->nclients, queue->nsockets);
        puts("sq_update_arr : invalid queue->nsockets");
        exit(-1);
    }

    return 0;
}

int sq_free(socket_queue *queue)
{
	if (queue == NULL) return SQ_NULL_FLAG;
	socket_queue_node *temp;
	for (socket_queue_node *curr = queue->head; curr != NULL;)
	{
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(queue->pollarr);
	pthread_mutex_destroy(&(queue->mutex));
	pthread_cond_destroy(&(queue->full));
	free(queue);

	return 0;
}