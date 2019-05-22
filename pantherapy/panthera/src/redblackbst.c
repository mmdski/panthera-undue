#include <panthera/cii/assert.h>
#include <panthera/redblackbst.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct TreeNode TreeNode;

enum color { BLACK, RED };

struct TreeNode {

    void *key;
    void *value;

    int  size;
    bool color; /* red or black */

    TreeNode *l; /* left */
    TreeNode *r; /* right */
};

static TreeNode *
tree_node_new (void *key, void *value)
{
    TreeNode *node;
    NEW (node);
    node->size  = 1;
    node->color = RED;
    node->key   = key;
    node->value = value;
    node->l     = NULL;
    node->r     = NULL;
    return node;
}

static void
tree_node_free (TreeNode *node)
{
    FREE (node);
}

static void
tree_free (TreeNode *node)
{
    if (node) {
        tree_free (node->l);
        tree_free (node->r);
        tree_node_free (node);
    }
}

static int
tree_size (TreeNode *node)
{
    if (node == NULL)
        return 0;
    else
        return node->size;
}

static TreeNode *
tree_min (TreeNode *node)
{
    if (node->l == NULL)
        return node;
    else
        return tree_min (node->l);
}

static TreeNode *
tree_max (TreeNode *node)
{
    if (node->r == NULL)
        return node;
    else
        return tree_max (node->r);
}

static TreeNode *
tree_get (TreeNode *node, KeyCompareFunc compare_func, void *key)
{
    if (node == NULL)
        return NULL;

    if (compare_func (key, node->key) < 0)
        return tree_get (node->l, compare_func, key);
    else if (compare_func (key, node->key) > 0)
        return tree_get (node->r, compare_func, key);
    else
        return node;
}

static bool
tree_contains (TreeNode *node, KeyCompareFunc compare_func, void *key)
{
    return tree_get (node, compare_func, key) != NULL;
}

static int
tree_is_red (TreeNode *node)
{
    return node != NULL && node->color;
}

static TreeNode *
tree_rotate_right (TreeNode *h)
{
    assert (h);
    assert (h->l->color == RED);

    TreeNode *x = h->l;
    h->l        = x->r;
    x->r        = h;
    x->color    = x->r->color;
    x->r->color = RED;
    x->size     = h->size;
    h->size     = tree_size (h->l) + tree_size (h->r) + 1;
    return x;
}

static TreeNode *
tree_rotate_left (TreeNode *h)
{
    assert (h);
    assert (h->r->color == RED);

    TreeNode *x = h->r;
    h->r        = x->l;
    x->l        = h;
    x->color    = x->l->color;
    x->l->color = RED;
    x->size     = h->size;
    h->size     = tree_size (h->l) + tree_size (h->r) + 1;
    return x;
}

/* flip the colors of a node and its two children */
static void
tree_flip_colors (TreeNode *node)
{
    assert (node && node->l && node->r);
    node->color    = !(node->color);
    node->l->color = !(node->l->color);
    node->r->color = !(node->r->color);
}

/* restore red-black tree invariant */
static TreeNode *
tree_balance (TreeNode *node)
{
    assert (node);

    if (tree_is_red (node->r))
        node = tree_rotate_left (node);
    if (tree_is_red (node->l) && tree_is_red (node->l->l))
        node = tree_rotate_right (node);
    if (tree_is_red (node->l) && tree_is_red (node->r))
        tree_flip_colors (node);

    node->size = tree_size (node->l) + tree_size (node->r) + 1;
    return node;
}

/* assuming that node is red and both node->left and node->left->left are
 * black, make node->left or one of its children red.
 */
static TreeNode *
tree_move_red_left (TreeNode *node)
{
    assert (node);
    assert (tree_is_red (node) && !tree_is_red (node->l) &&
            !tree_is_red (node->l->l));

    tree_flip_colors (node);
    if (tree_is_red (node->r->l)) {
        node->r = tree_rotate_right (node->r);
        node    = tree_rotate_left (node);
        tree_flip_colors (node);
    }
    return node;
}

/* assuming that node is red and both node->right and node->right->left are
 * black, make node->right or one of its children red.
 */
static TreeNode *
tree_move_red_right (TreeNode *node)
{
    assert (node);
    assert (tree_is_red (node) && !tree_is_red (node->r) &&
            !tree_is_red (node->r->l));

    tree_flip_colors (node);
    if (tree_is_red (node->l->l)) {
        node = tree_rotate_right (node);
        tree_flip_colors (node);
    }
    return node;
}

/* delete the node with the minimum x rooted at node */
static TreeNode *
tree_delete_min (TreeNode *node)
{

    if (node->l == NULL) {
        tree_node_free (node);
        return NULL;
    }

    if (!tree_is_red (node->l) && !tree_is_red (node->l->l))
        node = tree_move_red_left (node);

    node->l = tree_delete_min (node->l);
    return tree_balance (node);
}

/* delete the node with the given key rooted at node */
static TreeNode *
tree_delete (TreeNode *node, KeyCompareFunc compare_func, void *key)
{
    assert (tree_get (node, key, compare_func) != NULL);

    if (key < node->key) {
        if (!tree_is_red (node->l) && !tree_is_red (node->l->l))
            node = tree_move_red_left (node);
        node->l = tree_delete (node->l, compare_func, key);
    } else {
        if (tree_is_red (node->l))
            node = tree_rotate_right (node);
        if (key == node->key && node->r == NULL) {
            tree_node_free (node);
            return NULL;
        }
        if (!tree_is_red (node->r) && !tree_is_red (node->r->l))
            node = tree_move_red_right (node);
        if (key == node->key) {
            TreeNode *min_r = tree_min (node->r);
            node->key       = min_r->key;
            node->value     = min_r->value;
            min_r->value    = NULL;
            node->r         = tree_delete_min (node->r);
        } else
            node->r = tree_delete (node->r, compare_func, key);
    }
    return tree_balance (node);
}

static int
tree_keys (TreeNode *node, int i, void **key_array)
{
    if (node) {
        i                  = tree_keys (node->l, i, key_array);
        *(key_array + i++) = node->key;
        i                  = tree_keys (node->r, i, key_array);
    }
    return i;
}

static TreeNode *
tree_put (TreeNode *node, KeyCompareFunc compare_func, void *key, void *value)
{
    if (!node)
        return tree_node_new (key, value);

    if (compare_func (key, node->key) < 0)
        node->l = tree_put (node->l, compare_func, key, value);
    else if (compare_func (key, node->key) > 0)
        node->r = tree_put (node->r, compare_func, key, value);
    else {
        node->value = value;
    }

    /* fix any right-leaning links */
    if (tree_is_red (node->r) && !tree_is_red (node->l))
        node = tree_rotate_left (node);
    if (tree_is_red (node->l) && tree_is_red (node->l->l))
        node = tree_rotate_right (node);
    if (tree_is_red (node->l) && (tree_is_red (node->r)))
        tree_flip_colors (node);
    node->size = 1 + tree_size (node->l) + tree_size (node->r);
    return node;
}

struct RedBlackBST {
    TreeNode *     root;
    KeyCompareFunc compare_func;
};

RedBlackBST
redblackbst_new (KeyCompareFunc compare_func)
{
    RedBlackBST tree;
    NEW (tree);
    tree->root         = NULL;
    tree->compare_func = compare_func;
    return tree;
}

void
redblackbst_free (RedBlackBST tree)
{
    assert (tree);
    tree_free (tree->root);
    FREE (tree);
}

int
redblackbst_size (RedBlackBST tree)
{
    assert (tree);
    return tree_size (tree);
}

void *
redblackbst_min_key (RedBlackBST tree)
{
    assert (tree);
    assert (tree_size (tree) > 0);
    TreeNode *min = tree_min (tree->root);
    return min->key;
}

void *
redblackbst_max_key (RedBlackBST tree)
{
    assert (tree);
    assert (tree_size (tree) > 0);
    TreeNode *max = tree_max (tree->root);
    return max->key;
}

void *
redblackbst_get (RedBlackBST tree, void *key)
{
    assert (tree && key);
    TreeNode *node = tree_get (tree->root, key, tree->compare_func);
    if (node)
        return node->value;
    else
        return NULL;
}

void *
redblackbst_put (RedBlackBST tree, void *key, void *value)
{
    assert (tree && key && value);
    void *    old_value;
    TreeNode *node = tree_get (tree->root, key, tree->compare_func);
    if (node) {
        old_value   = node->value;
        node->value = value;
    } else {
        old_value  = NULL;
        tree->root = tree_put (tree->root, tree->compare_func, key, value);
        tree->root->color = BLACK;
    }
    return old_value;
}

bool
redblackbst_contains (RedBlackBST tree, void *key)
{
    assert (tree && key);
    return tree_contains (tree->root, tree->compare_func, key);
}

void
redblackbst_delete (RedBlackBST tree, void *key)
{
    assert (tree && key);
    if (!tree_contains (tree->root, tree->compare_func, key))
        return;

    /* if both children of root are black, set root to red */
    if (!tree_is_red (tree->root->l) && !tree_is_red (tree->root->r))
        tree->root->color = RED;

    tree->root = tree_delete (tree->root, tree->compare_func, key);
    if (redblackbst_size (tree) > 0)
        tree->root->color = BLACK;
}

void
redblackbst_keys (RedBlackBST tree, void **keys)
{
    assert (tree && keys);
    tree_keys (tree->root, 0, keys);
}
