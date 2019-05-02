#include <cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/xstable.h>
#include <stddef.h>

typedef struct TreeNode TreeNode;

enum color { BLACK, RED };

struct TreeNode {
    int          key;
    CrossSection value;

    int  size;
    bool color; /* red or black */

    TreeNode *l; /* left */
    TreeNode *r; /* right */
};

static TreeNode *
tree_node_new (int key, CrossSection value)
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
    if (node->value)
        xs_free (node->value);
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
tree_get (TreeNode *node, int key)
{
    if (node == NULL)
        return NULL;

    if (key < node->key)
        return tree_get (node->l, key);
    else if (key > node->key)
        return tree_get (node->r, key);
    else
        return node;
}

static bool
tree_contains (TreeNode *node, int key)
{
    return tree_get (node, key) != NULL;
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
tree_delete (TreeNode *node, int key)
{
    assert (tree_get (node, key) != NULL);

    if (key < node->key) {
        if (!tree_is_red (node->l) && !tree_is_red (node->l->l))
            node = tree_move_red_left (node);
        node->l = tree_delete (node->l, key);
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
            xs_free (node->value);
            node->value  = min_r->value;
            min_r->value = NULL;
            node->r      = tree_delete_min (node->r);
        } else
            node->r = tree_delete (node->r, key);
    }
    return tree_balance (node);
}

static int
tree_keys (TreeNode *node, int i, int *key_array)
{
    if (node) {
        i                  = tree_keys (node->l, i, key_array);
        *(key_array + i++) = node->key;
        i                  = tree_keys (node->r, i, key_array);
    }
    return i;
}

static TreeNode *
tree_put (TreeNode *node, int key, CrossSection value)
{
    if (!node)
        return tree_node_new (key, value);

    if (key < node->key)
        node->l = tree_put (node->l, key, value);
    else if (key > node->key)
        node->r = tree_put (node->r, key, value);
    else {
        xs_free (node->value);
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

struct XSTable {
    TreeNode *root;
};

XSTable
xstable_new (void)
{
    XSTable xstable;
    NEW (xstable);

    xstable->root = NULL;
    return xstable;
}

void
xstable_free (XSTable xstable)
{
    tree_free (xstable->root);
    FREE (xstable);
}

int
xstable_size (XSTable xstable)
{
    return tree_size (xstable->root);
}

double
xstable_min_x (XSTable xstable)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    if (xstable_size (xstable) == 0)
        RAISE (empty_table_error);

    TreeNode *min = tree_min (xstable->root);
    return min->key;
}

double
xstable_max_key (XSTable xstable)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    if (xstable_size (xstable) == 0)
        RAISE (empty_table_error);

    TreeNode *max = tree_max (xstable->root);
    return max->key;
}

CrossSection
xstable_get (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    TreeNode *node = tree_get (xstable->root, key);
    if (node)
        return node->value;
    else
        return NULL;
}

void
xstable_put (XSTable xstable, int key, CrossSection xs)
{
    if (!xstable || !xs)
        RAISE (null_ptr_arg_error);

    xstable->root        = tree_put (xstable->root, key, xs);
    xstable->root->color = BLACK;
}

bool
xstable_contains (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    return tree_contains (xstable->root, key);
}

void
xstable_delete (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    if (!tree_contains (xstable->root, key))
        return;

    /* if both children of root are black, set root to red */
    if (!tree_is_red (xstable->root->l) && !tree_is_red (xstable->root->r))
        xstable->root->color = RED;

    xstable->root = tree_delete (xstable->root, key);
    if (xstable_size (xstable) > 0)
        xstable->root->color = BLACK;
}

int
xstable_keys (XSTable xstable, int **keys)
{
    if (!xstable || !keys)
        RAISE (null_ptr_arg_error);

    int size = tree_size (xstable->root);

    *keys = Mem_calloc (size, sizeof (int), __FILE__, __LINE__);
    tree_keys (xstable->root, 0, *keys);

    return size;
}
