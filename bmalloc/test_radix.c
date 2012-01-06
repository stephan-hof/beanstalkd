#include "radix.h"
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
    size_t pagesize, pagebits;
    radix_tree_t *tree;
    pagesize = getpagesize();
    for(pagebits=0; pagebits<32; pagebits++) {
        if ( (1 << pagebits ) == pagesize ) {
            break;
        }
    }

    int bits = (sizeof(uintptr_t)*8);
    tree = create_radix_tree(bits - pagebits);
    char foo[] = "Slayer fucked my hearing";
    unsigned long i,j;
    for(i=0; i < 1048575; i++) {
        radix_tree_set(tree, i, foo);
    }

    for(j=0; j < 100; j++) {
        for(i=0; i < 1048575; i++) {
            radix_tree_get(tree, i);
        }
    }

    free_radix_tree(tree, bits-pagebits);

    return 1;
}

