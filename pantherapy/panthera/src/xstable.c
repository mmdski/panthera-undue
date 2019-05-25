#include <panthera/cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/redblackbst.h>
#include <panthera/xstable.h>
#include <stddef.h>

struct XSTable {
    RedBlackBST tree;
};

int
xs_key_compare_func (const void *x, const void *y)
{
    int x_val = *(int *) x;
    int y_val = *(int *) y;

    if (x_val < y_val)
        return -1;
    else if (x_val > y_val)
        return 1;
    else
        return 0;
}

XSTable
xstable_new (void)
{
    XSTable xstable;
    NEW (xstable);
    xstable->tree = redblackbst_new (&xs_key_compare_func);
    return xstable;
}

void
xstable_free (XSTable xstable)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    int    size = xstable_size (xstable);
    int    i;
    int    k;
    void **keys = Mem_calloc (size, sizeof (void *), __FILE__, __LINE__);
    redblackbst_keys (xstable->tree, keys);
    for (i = 0; i < size; i++) {
        k = *(int *) *(keys + i);
        xstable_delete (xstable, k);
    }
    redblackbst_free (xstable->tree);
    FREE (xstable);
    Mem_free (keys, __FILE__, __LINE__);
}

int
xstable_size (XSTable xstable)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    return redblackbst_size (xstable->tree);
}

CrossSection
xstable_get (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);

    CrossSection xs;

    Item *item = redblackbst_get (xstable->tree, &key);
    if (item) {
        xs = (CrossSection) item->value;
        redblackbst_free_item (item);
    } else
        xs = NULL;
    return xs;
}

void
xstable_put (XSTable xstable, int key, CrossSection xs)
{
    if (!xstable || !xs)
        RAISE (null_ptr_arg_error);

    int *tree_key;

    Item *item = redblackbst_get (xstable->tree, &key);
    if (item) {
        tree_key = item->key;
        redblackbst_free_item (item);
    } else {
        tree_key  = Mem_alloc (sizeof (int), __FILE__, __LINE__);
        *tree_key = key;
    }

    redblackbst_put (xstable->tree, tree_key, xs);
}

bool
xstable_contains (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    return redblackbst_contains (xstable->tree, &key);
}

void
xstable_delete (XSTable xstable, int key)
{
    if (!xstable)
        RAISE (null_ptr_arg_error);
    if (!xstable_contains (xstable, key))
        return;
    Item *item = redblackbst_get (xstable->tree, &key);
    redblackbst_delete (xstable->tree, &key);
    if (item) {
        Mem_free (item->key, __FILE__, __LINE__);
        redblackbst_free_item (item);
    }
}

int
xstable_keys (XSTable xstable, int **keys)
{
    if (!xstable || !keys)
        RAISE (null_ptr_arg_error);

    int    i;
    int    size = redblackbst_size (xstable->tree);
    int *  k;
    void **tree_keys = Mem_calloc (size, sizeof (void *), __FILE__, __LINE__);

    k = Mem_calloc (size, sizeof (int), __FILE__, __LINE__);
    redblackbst_keys (xstable->tree, tree_keys);
    for (i = 0; i < size; i++) {
        *(k + i) = *(int *) *(tree_keys + i);
    }

    *keys = k;
    Mem_free (tree_keys, __FILE__, __LINE__);

    return size;
}
