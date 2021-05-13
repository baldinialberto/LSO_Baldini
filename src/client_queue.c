#include <client_queue.h>

int sq_is_empty(socket_queue *queue)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    
    return queue->nclients == 0;
}
int sq_push(socket_queue *queue, int client_socket, int flag)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    if (client_socket < 0) return SQ_INVALID_FLAG;

    socket_queue_node* newclient = malloc(sizeof(socket_queue_node));
    newclient->client_socket = client_socket;
    newclient->status = SQ_LIVE_FLAG;

    queue->tail->next = newclient;
    newclient->next = NULL;
    queue->tail = newclient;

    queue->nclients++;

    queue->pollarr_valid = 0;

    return 0;
}
int sq_remove(socket_queue *queue, int client_socket)
{
    if (queue == NULL) return SQ_NULL_FLAG;
    if (client_socket < 0) return SQ_INVALID_FLAG;
    if (sq_is_empty(queue)) return SQ_EMPTY_FLAG;

    socket_queue_node *curr = queue->head, *prev = NULL;
    
    while (curr->client_socket != client_socket 
        && queue->tail != curr) // linear scan
    {
        prev = curr;
        curr = curr->next;
    }
    if (curr->client_socket == client_socket) // found
    {
        if (curr == queue->head && curr == queue->tail) // remove last client
        {
            queue->head = (queue->tail = NULL);
            free(curr);
        } else if (curr == queue->head) // remove head
        {
            queue->head = curr->next;
            free(curr);  
        } else if (curr == queue->tail) // remove tail
        {
            queue->tail = prev;
            free(curr);
        } else {                        // remove in between
            prev->next = curr->next;
            free(curr);
        }

        queue->nclients--;
        queue->pollarr_valid = 0;

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
            queue->nclients * sizeof(struct pollfd)), 
        NULL, "sq_update_arr : realloc"
    )
    int i = 0;
    for (socket_queue_node* node = queue->head; node != NULL; node = node->next)
    {
       (queue->pollarr)[i].fd = node->client_socket;
       (queue->pollarr)[i++].events = POLLIN | POLLHUP;
    }
    queue->pollarr_valid = 1;
    
    if (i != queue->nclients)
    {
        puts("sq_update_arr : invalid queue->nclients");
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