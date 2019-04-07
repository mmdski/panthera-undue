#include <cii/mem.h>
#include <panthera/reach.h>
#include <stdbool.h>

typedef struct TreeNode TreeNode;

enum color {BLACK, RED};

struct TreeNode {
    double x;        /* key */
    CrossSection xs; /* value */

    int size;
    bool color; /* red or black */

    TreeNode *l; /* left */
    TreeNode *r; /* right */
};

static TreeNode *tree_node_new(double x, CrossSection xs) {
    TreeNode *node;
    NEW(node);
    node->size  = 1;
    node->color = BLACK;
    node->x     = x;
    node->xs    = xs;
    node->l     = NULL;
    node->r     = NULL;
    return node;
}

static void tree_node_free(TreeNode *node) {
    xs_free(node->xs);
    FREE(node);
}

static void tree_free(TreeNode *node) {
    if (node) {
        tree_free(node->l);
        tree_free(node->r);
        tree_node_free(node);
    }
}

static int tree_size(TreeNode *node) {
    if (node == NULL)
        return 0;
    else
        return node->size;
}

static int tree_is_red(TreeNode *node) {
    return node != NULL && node->color;
}

static TreeNode *tree_rotate_right(TreeNode *h) {
    assert(!h);
    assert(h->l->color == RED);

    TreeNode *x = h->l;
    h->l = x->r;
    x->r = h;
    x->color = x->r->color;
    x->r->color = RED;
    x->size = h->size;
    h->size = tree_size(h->l) + tree_size(h->r) + 1;
    return x;
}

static TreeNode *tree_rotate_left(TreeNode *h) {
    assert(!h);
    assert(h->r->color == RED);

    TreeNode *x = h->l;
    h->r = x->l;
    x->l = h;
    x->color = x->l->color;
    x->l->color = RED;
    x->size = h->size;
    h->size = tree_size(h->l) + tree_size(h->r) + 1;
    return x;
}

static void tree_flip_colors(TreeNode *node) {
    node->color = !(node->color);
    node->l->color = !(node->l->color);
    node->r->color = !(node->r->color);
}

static CrossSection tree_get(TreeNode *node, double x) {
    if (node == NULL)
        return NULL;

    if (x < node->x)
        return tree_get(node->l, x);
    else if (x > node->x)
        return tree_get(node->r, x);
    else
        return node->xs;
}

static int tree_keys(TreeNode* node, int i, double *x_array) {
    if (node) {
        i = tree_keys(node->l, i, x_array);
        *(x_array + i++) = node->x;
        i = tree_keys(node->r, i, x_array);
    }
    return i;
}

static TreeNode *tree_put(TreeNode* node, double x, CrossSection xs) {
    if (!node)
        return tree_node_new(x, xs);

    if (x < node->x)
        node->l = tree_put(node->l, x, xs);
    else if (x > node->x)
        node->r = tree_put(node->r, x, xs);
    else {
        xs_free(node->xs);
        node->xs = xs;
    }

    /* fix any right-leaning links */
    if (tree_is_red(node->r) && !tree_is_red(node->l))
        node = tree_rotate_left(node);
    if (tree_is_red(node->l) && tree_is_red((node->l)->l))
        node = tree_rotate_right(node);
    if (tree_is_red(node->l) && (tree_is_red(node->r)))
        tree_flip_colors(node);
    node->size = 1 + tree_size(node->l) + tree_size(node->r);
    return node;
}

struct Reach {
    TreeNode *root;
};

Reach reach_new(void) {
    Reach reach;
    NEW(reach);

    reach->root = NULL;

    return reach;
}

void reach_free(Reach reach) {
    tree_free(reach->root);
    FREE(reach);
}

int reach_size(Reach reach) {
    return tree_size(reach->root);
}

CrossSection reach_get(Reach reach, double x) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    return tree_get(reach->root, x);
}

void reach_put(Reach reach, double x, CrossSection xs) {
    if (!reach || !xs)
        RAISE(null_ptr_arg_Error);

    reach->root = tree_put(reach->root, x, xs);
    reach->root->color = BLACK;
}

int reach_stream_distance(Reach reach, double **x) {
    if (!reach || !x)
        RAISE(null_ptr_arg_Error);

    int size = tree_size(reach->root);

    *x = Mem_calloc(size, sizeof(double), __FILE__, __LINE__);
    tree_keys(reach->root, 0, *x);

    return size;
}
