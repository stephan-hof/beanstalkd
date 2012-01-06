#ifndef FREE_LIST_H
#define FREE_LIST_H

typedef struct free_node {
    struct free_node * next;
    struct free_node * prev;
} free_node_t;

typedef struct free_list {
    unsigned int items;
    free_node_t * head;
} free_list_t;

free_list_t* create_free_list();
void add_node(free_list_t *list, void * data);
void * remove_node(free_list_t *list, void *data);
void * pop_node(free_list_t *list);

#define IS_EMTPY(list) (!(list->items))

#endif /*FREE_LIST_H*/
