#include <stdlib.h>
#include "free_list.h"

free_list_t *
create_free_list()
{
    free_list_t *new = malloc(sizeof(free_list_t));
    /*
     * sentinal, which starts a cirular doubled linked list.
     * This avoids annoying 'if' checks when a node is added or removed
     */
    new->head = malloc(sizeof(free_node_t));
    new->head->next = new->head;
    new->head->prev = new->head;
    /*
     * Cause of the sentinal, this is needed to know if
     * the list is empty or not
     */
    new->items = 0;
    return new;
}

void
add_node(free_list_t *list, void * data)
{
    free_node_t * node = (free_node_t*) data;

    list->head->prev->next = node;
    node->prev = list->head->prev;

    node->next = list->head;
    list->head->prev = node;

    list->head = node;
    list->items += 1;
}

void *
remove_node(free_list_t *list, void *data)
{
    free_node_t * node = (free_node_t*) data;
    node->prev->next = node->next;
    node->next->prev = node->prev;
    list->items -= 1;
    if ( node == list->head ) {
        list->head = node->next;
    }
    return node;
}

void *
pop_node(free_list_t *list)
{
    return remove_node(list, list->head);
}
