#include <common.h>

int string_compare(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}
list_node *list_allocnode(void *data)
{
    list_node *newNode = NULL;
    CHECK_BADVAL_PERROR_EXIT(
        newNode = (list_node *)malloc(sizeof(list_node)), 
        NULL, "list_allocnode : calloc"
    );
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}
int list_insert(ln_ptr *list, list_node *node, int (*comp)(void*, void *))
{
    list_node *curr = *list, *prev = NULL;
    while (curr != NULL && comp((void *)node->data, (void*)curr->data) > 0)
    {
        prev = curr;
        curr = curr->next;
    }
    if (prev == NULL)
    {
        node->next = curr;
        *list = node;
    }
    else
    {
        prev->next = node;
        node->next = curr;
    }
    return 0;
}

int list_foreach(ln_ptr list, void (*proc)(void *))
{
    while (list != NULL)
    {
        proc((void *)list);
        list = list->next;
    }
    return 0;
}

int list_free(ln_ptr *list)
{
    ln_ptr temp = *list, next;
    while (temp != NULL)
    {   
        next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
    }
    return 0;
}