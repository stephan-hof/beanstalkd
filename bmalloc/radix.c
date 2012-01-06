#include <stdlib.h>
#include "radix.h"

radix_tree_t *
create_radix_tree(int bits)
{
    int root_len;
    radix_tree_t * tree = malloc(sizeof(radix_tree_t));

    tree->level2_bits = (bits + 2) / 3;
    tree->level2_length = 1 << tree->level2_bits;

    tree->level3_bits = bits - (2 * tree->level2_bits);
    tree->level3_length = 1 << tree->level3_bits;

    root_len = 1 << (bits - (tree->level2_bits + tree->level3_bits));

    tree->level2_bitmask = tree->level2_length-1;
    tree->level3_bitmask = tree->level3_length-1;

    tree->root = calloc(root_len, sizeof(void*));
    return tree;
}

void
free_radix_tree(radix_tree_t *tree, int bits)
{
    int root_len = 1 << (bits - (tree->level2_bits + tree->level3_bits));

    int i,j;
    for(i=0; i < root_len; i++) {
        if( tree->root[i] != NULL ) {
            for(j=0; j < tree->level2_length; j++) {
                if( tree->root[i][j] != NULL) {
                    free(tree->root[i][j]);
                }
            }
            free(tree->root[i]);
        }
    }
    free(tree->root);
    free(tree);
}

int
radix_tree_set(radix_tree_t *tree, unsigned int key, void *data)
{

    const unsigned int l1_key = key >> (tree->level2_bits + tree->level3_bits);
    const unsigned int l2_key = (key >> tree->level3_bits) & (tree->level2_bitmask);
    const unsigned int l3_key = key & (tree->level3_bitmask);

    if(tree->root[l1_key] == NULL) {
        tree->root[l1_key] = calloc(tree->level2_length, sizeof(void*));
    }

    if(tree->root[l1_key][l2_key] == NULL) {
        tree->root[l1_key][l2_key] = calloc(tree->level3_length, sizeof(void*));
    }

    tree->root[l1_key][l2_key][l3_key] = data;
    return 1;
}

void *
radix_tree_get(radix_tree_t *tree, unsigned int key)
{
    const unsigned int l1_key = key >> (tree->level2_bits + tree->level3_bits);
    const unsigned int l2_key = (key >> tree->level3_bits) & (tree->level2_bitmask);
    const unsigned int l3_key = key & (tree->level3_bitmask);

    return tree->root[l1_key][l2_key][l3_key];
}
