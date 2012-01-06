#ifndef RADIX_H
#define RADIX_H

typedef struct radix_tree_t {
    /* bits for level1/root are implictly calcualted */

    int level2_bits;
    int level2_length;

    int level3_bits;
    int level3_length;

    int level2_bitmask;
    int level3_bitmask;

    void ****root;
} radix_tree_t;

radix_tree_t * create_radix_tree(int bits);
void free_radix_tree(radix_tree_t *tree, int bits);
int radix_tree_set(radix_tree_t *tree, unsigned int key, void *data);
void * radix_tree_get(radix_tree_t *tree, unsigned int key);
#endif /*RADIX_H*/
