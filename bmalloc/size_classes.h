#include "free_list.h"
#include <stddef.h>

#ifndef SIZE_CLASSES_H
#define SIZE_CLASSES_H

typedef struct size_class {
    unsigned int max_size;
    unsigned int used_blocks;
    unsigned int block_size;
    free_list_t *free;
} size_class_t;


unsigned int get_class_index(size_t size);
size_class_t * create_size_classes();

#endif /*SIZE_CLASSES_H*/
