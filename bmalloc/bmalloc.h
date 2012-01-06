#ifndef BMALLOC_H
#define BMALLOC_H

#include <stddef.h>
#include "radix.h"
#include "size_classes.h"

typedef struct block {
    unsigned int used_bytes;
    size_class_t *cls;
    void *data;
} block_t;


typedef struct bmalloc_handler {
    size_class_t *classes;
    radix_tree_t *tree;
} bmalloc_handler_t;

void * bmalloc(bmalloc_handler_t *handler, size_t size);
void bmalloc_free(bmalloc_handler_t *handler, void *ptr);
bmalloc_handler_t * create_bmalloc_handler();
void free_handler(bmalloc_handler_t *handler);

#endif /*BMALLOC_H*/
