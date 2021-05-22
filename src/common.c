#include <common.h>

list_node *list_allocnode(void *data)
{
    list_node *newNode = NULL;
    CHECK_BADVAL_PERROR_EXIT(
        newNode = (list_node *)calloc(1, sizeof(list_node)), 
        NULL, "list_allocnode : calloc"
    );
    newNode->data = data;
    return newNode;
}
int list_insert(ln_ptr *list, list_node *node, int (*comp)(void*, void *))
{
    list_node *curr = *list, *prev = NULL;
    while (curr != NULL && comp((void *)node, (void*)curr) > 0)
    {
        prev = curr;
        curr = curr->next;
    }
    if (prev == curr)
    {
        *list = node;
    }
    else
    {
        if (prev != NULL) prev->next = node;
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
    list_foreach(*list, free);
    *list = NULL;
    return 0;
}