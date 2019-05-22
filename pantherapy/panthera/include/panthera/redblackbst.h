#ifndef REDBLACKBST_INCLUDED
#define REDBLACKBST_INCLUDED

typedef int (*KeyCompareFunc) (void *x, void *y);

typedef struct RedBlackBST *RedBlackBST;

extern RedBlackBST
redblackbst_new (KeyCompareFunc compare_func);

extern void
redblackbst_free (RedBlackBST tree);

extern int
redblackbst_size (RedBlackBST tree);

extern void *
redblackbst_min_key (RedBlackBST tree);

extern void *
redblackbst_max_key (RedBlackBST tree);

extern void *
redblackbst_put (RedBlackBST tree, void *key, void *value);

extern bool
redblackbst_contains (RedBlackBST tree, void *key);

extern void
redblackbst_delete (RedBlackBST tree, void *key);

void
redblackbst_keys (RedBlackBST tree, void **keys);

#endif
