#include <client_queue.h>

int cq_is_empty(client_queue *queue)
{
    if (queue == NULL) return CQ_NULL_FLAG;
    
    return queue->head == NULL && queue->tail == NULL;
}
int cq_push(client_queue *queue, int client_socket)
{
    if (queue == NULL) return CQ_NULL_FLAG;
    if (client_socket < 0) return CQ_INVALID_FLAG;

    client_queue_node* newclient = malloc(sizeof(client_queue_node));
    newclient->client_socket = client_socket;
    newclient->status = CQ_LIVE_FLAG;

    queue->tail->next = newclient;
    newclient->next = NULL;
    queue->tail = newclient;

    return 0;
}
int cq_remove(client_queue *queue, int client_socket)
{
    if (queue == NULL) return CQ_NULL_FLAG;
    if (client_socket < 0) return CQ_INVALID_FLAG;
    if (cq_is_empty(queue)) return CQ_EMPTY_FLAG;

    client_queue_node *curr = queue->head, *prev = NULL;
    
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

        return 0;
    }

    return CQ_NOTFOUND_FLAG;
}
int cq_serve(client_queue *queue)
{
    if (queue == NULL) return -CQ_NULL_FLAG;
    if (cq_is_empty(queue)) return -CQ_EMPTY_FLAG;

    int res = queue->head->client_socket;
    queue->head->status |= CQ_SERVING_FLAG;
    
    if (queue->head->next != NULL)  // move head to tail and shift 
    {
        queue->tail->next = queue->head;
        queue->head = queue->head->next;
        queue->tail = queue->tail->next;
        queue->tail->next = NULL;
    }

    return res;
}