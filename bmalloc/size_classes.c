#include <stdlib.h>
#include <unistd.h>
#include "free_list.h"
#include "bmalloc.h"

#define SMALL_SIZE 1024
#define SMALL_SIZE_ARRAY_OFFSET 6

#define BIG_SIZE_BITS 10
static unsigned int BIG_SIZE_SIZE = 1 << BIG_SIZE_BITS;

#define MIN_BLOCK_SIZE (1024*1024)

static unsigned int
get_pos(size_t size)
{
    if ( size <= 16) return 0;
    if ( size <= 32) return 1;
    if ( size <= 64) return 2;
    if ( size <= 128) return 3;
    if ( size <= 256) return 4;
    if ( size <= 512) return 5;
    return 6;
}

unsigned int
get_class_index(size_t size)
{
    unsigned int pos;
    if (size <= SMALL_SIZE) {
        return get_pos(size);
    }

    pos = size >> BIG_SIZE_BITS;
    if (!(size & (BIG_SIZE_SIZE-1)))
        pos -= 1;
    return pos + SMALL_SIZE_ARRAY_OFFSET;
}

static int
is_good_block_size(unsigned int block_size, size_t size)
{
    if (block_size < MIN_BLOCK_SIZE) return 0;
    if (block_size < (size*4)) return 0;
    if (block_size % getpagesize()) return 0;
    return 1;
}

static unsigned int
calc_block_size(size_t size)
{
    unsigned int block_size = 0;
    while(!is_good_block_size(block_size, size)){
        block_size += size;
    }
    return block_size;
}

static size_class_t *
add_size_class(size_class_t *classes, unsigned int counter, size_t size)
{
    classes = realloc(classes, (counter+1) * sizeof(size_class_t));
    classes[counter].max_size = size;
    classes[counter].free = create_free_list();
    classes[counter].used_blocks = 0;
    classes[counter].block_size = calc_block_size(size);
    return classes;
}

size_class_t *
create_size_classes()
{
    unsigned int max_size_class = BIG_SIZE_SIZE * 128;
    unsigned int i;
    unsigned int class_counter=0;
    size_class_t *classes = NULL;

    for(i=16; i <= SMALL_SIZE; i=i<<1) {
        classes = add_size_class(classes, class_counter, i);
        class_counter += 1;
    }

    for(i=SMALL_SIZE + BIG_SIZE_SIZE; i <= max_size_class ; i+= BIG_SIZE_SIZE) {
        classes = add_size_class(classes, class_counter, i);
        class_counter += 1;
    }

    return classes;
}
