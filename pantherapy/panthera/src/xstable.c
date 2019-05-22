#include <panthera/cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/redblackbst.h>
#include <panthera/xstable.h>
#include <stddef.h>

struct XSTable {
    RedBlackBST tree;
};

int
xs_key_compare_func (void *x, void *y)
{
    if (*(int *) x < *(int *) y)
        return -1;
    else if (*(int *) x > *(int *) y)
        return 1;
    else
        return 0;
}

XSTable
xstable_new (void)
{
    XSTable xstable;
    NEW (xstable);

    xstable->tree = redblackbst_new (xs_key_compare_func);
    return xstable;
}

void
xstable_free (XSTable xstable)
{
    int size;
    redblackbst_free (xstable->tree);
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
