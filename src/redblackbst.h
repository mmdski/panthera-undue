#ifndef REDBLACKBST_INCLUDED
#define REDBLACKBST_INCLUDED
#include <stdbool.h>

/**
 * SECTION: Red black binary search tree
 * @short_description: Red-black BST
 * @title: RedBlackBST
 *
 * Red black binary search tree
 */

/**
 * KeyCompareFunc:
 * @x: x key
 * @y: y key
 *
 * Key compare function.
 *
 * This function is used to place items in the appropriate location in the
 * tree.
 *
 * Returns: -1 if x < y, 1 if x > y, 0 if x == y
 */

typedef int (*KeyCompareFunc) (const void *x, const void *y);

/**
 * RecBlackBST:
 *
 * Red-black binary search tree.
 */
typedef struct RedBlackBST *RedBlackBST;

/**
 * Item:
 * @key:   Key of item
 * @value: Value of item
 *
 * Key, value pair returned by redblackbst_get().
 */
typedef struct {
    void *key;
    void *value;
} Item;

/**
 * redblackbst_new:
 * @compare_func: a #KeyCompareFunc
 *
 * Creates a new #RedBlackBST. The tree must be freed with redblackbst_free().
 *
 * Returns: a new search tree
 */
extern RedBlackBST
redblackbst_new (KeyCompareFunc compare_func);

/**
 * redblackbst_free:
 * @tree: a #RedBlackBST
 *
 * Frees @tree, does not free the keys or the values contained in @tree.
 *
 * Returns: none
 */
extern void
redblackbst_free (RedBlackBST tree);

/**
 * redblackbst_free_item:
 * @item: an #Item
 *
 * Frees @item, does not free `key` or `value` members of @item
 *
 * Returns: None
 */
extern void
redblackbst_free_item (Item *item);

/**
 * redblackbst_size:
 * @tree: a #RedBlackBST
 *
 * Returns: the number of items in @tree
 */
extern int
redblackbst_size (RedBlackBST tree);

/**
 * redblackbst_min_key:
 * @tree: a #RedBlackBST
 *
 * Returns: the minimum key value in @tree
 */
extern void *
redblackbst_min_key (RedBlackBST tree);

/**
 * redblackbst_max_key:
 * @tree: a #RedBlackBST
 *
 * Returns: the maximum key value in @tree
 */
extern void *
redblackbst_max_key (RedBlackBST tree);

/**
 * redblackbst_get:
 * @tree: a #RedBlackBST
 * @key: a key
 *
 * If @key is matches a key contained in @tree, then this function returns an
 * #Item with references to the key, value pair contained in the tree node. The
 * returned item is newly created and must be freed with
 * redblackbst_free_item().
 *
 * Returns: an item with references to a key, value pair
 */
extern Item *
redblackbst_get (RedBlackBST tree, const void *key);

/**
 * redblackbst_put:
 * @tree: a #RedBlackBST
 * @key: a key
 * @value: a value
 *
 * Puts a key, value pair in @tree. If a node with a key equal to @key is
 * already contained in @tree, the corresponding value will be replaced by
 *  @value.
 *
 * Returns: None
 */
extern void
redblackbst_put (RedBlackBST tree, const void *key, void *value);

/**
 * redblackbst_contains:
 * @tree: a #RedBlackBST
 * @key: a key
 *
 * Returns true if @tree contains a key equal to @key, false otherwise.
 *
 * Returns: true if @tree contains @key, false otherwise
 */
extern bool
redblackbst_contains (RedBlackBST tree, const void *key);

/**
 * redblackbst_delete:
 * @tree: a #RedBlackBST
 * @key: a key
 *
 * Deletes the node in @tree with the key equal to @key. Nothing happens if
 * a @key is not found in @tree.
 *
 * Returns: None
 */
extern void
redblackbst_delete (RedBlackBST tree, const void *key);

/**
 * redblackbst_keys:
 * @tree: a #RedBlackBST
 * @keys: an array of `void *`
 *
 * Fills @keys with references to the keys contained in @tree. `keys` must be
 * previously allocated with the number of elements equal to the number of
 * items in @tree. Use redblackbst_size() to get the number of items in @tree.
 *
 * Returns: None
 */
extern void
redblackbst_keys (RedBlackBST tree, void **keys);

#endif
