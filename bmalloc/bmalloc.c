#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "radix.h"
#include "free_list.h"
#include "size_classes.h"
#include "bmalloc.h"

#define CACHED_BLOCKS 4
#define GET_SIZE_CLASS(handler, size) (&handler->classes[get_class_index(size)])

static size_t pagesize = 0;
static unsigned int pagebits = 0;

int mmap_prot = PROT_READ | PROT_WRITE;
int mmap_flags = MAP_PRIVATE | MAP_ANONYMOUS;

inline unsigned int
get_page_id(uintptr_t ptr)
{
    return ptr >> pagebits;
}

void
free_block_data(block_t *bl)
{
    munmap(bl->data, bl->cls->block_size);
}

int
allocate_block_data(block_t *bl)
{
    /*
     * No need for special alignment code, because while calculating the
     * block_size the pagesize is already considered
     */
    bl->data = mmap(NULL, bl->cls->block_size, mmap_prot, mmap_flags, -1, 0);

    if (bl->data == MAP_FAILED) {
        return -1;
    }
    return 1;
}

static int
allocate_new_blocks(bmalloc_handler_t *handler, size_class_t *cls)
{
    block_t *bl = malloc(sizeof(block_t));
    if (bl == NULL) return -1;

    bl->used_bytes = 0;
    bl->cls = cls;

    if (allocate_block_data(bl) == -1) {
        free(bl);
        return -1;
    }

    unsigned int i = 0;
    uintptr_t p = (uintptr_t) bl->data;
    for (i=0; i < (cls->block_size / pagesize); i++) {
        radix_tree_set(handler->tree, get_page_id(p) + i, bl);
    }

    void *begin = bl->data;

    while ( (begin + cls->max_size) < (bl->data + cls->block_size)) {
        add_node(cls->free, begin);
        begin += cls->max_size;
    }
    return 1;
}

void *
bmalloc(bmalloc_handler_t *handler, size_t size)
{
    void *ret;
    block_t *bl;
    if (size == 0) return NULL;

    size_class_t *cls = GET_SIZE_CLASS(handler, size);

    if (IS_EMTPY(cls->free)) {
       if (allocate_new_blocks(handler, cls) == -1) {
           return NULL;
       }
       cls->used_blocks += 1;
    }
    ret = pop_node(cls->free);
    bl = radix_tree_get(handler->tree, get_page_id((uintptr_t)ret));

    bl->used_bytes += cls->max_size;
    return ret;
}

static void
free_block(block_t *bl)
{
    void *begin = bl->data;
    while ((begin + bl->cls->max_size) < (bl->data + bl->cls->block_size)) {
        remove_node(bl->cls->free, begin);
        begin += bl->cls->max_size;
    }
    free_block_data(bl);
    free(bl);
}

void
bmalloc_free(bmalloc_handler_t *handler, void *ptr)
{
    if (ptr == NULL) return;

    block_t *bl = radix_tree_get(handler->tree, get_page_id((uintptr_t)ptr));
    add_node(bl->cls->free, (free_node_t*) ptr);

    bl->used_bytes -= bl->cls->max_size;
    if (bl->cls->used_blocks > CACHED_BLOCKS && bl->used_bytes == 0) {
        /*
         * need to do it before, becuase bl is
         * invalid after free_block
         */
        bl->cls->used_blocks -= 1;
        free_block(bl);
    }
}

bmalloc_handler_t *
create_bmalloc_handler()
{
    pagesize = getpagesize();
    for(pagebits=0; pagebits<32; pagebits++) {
        if ( (1 << pagebits ) == pagesize ) {
            break;
        }
    }

    bmalloc_handler_t *handler = malloc(sizeof(bmalloc_handler_t));
    handler->classes = create_size_classes();
    handler->tree = create_radix_tree((8*sizeof(uintptr_t))-pagebits);
    return handler;
}
