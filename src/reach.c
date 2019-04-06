#include <cii/mem.h>
#include <panthera/reach.h>

typedef struct ReachNode ReachNode;

struct ReachNode {
    int size;
    double x;
    CrossSection xs;

    ReachNode *l;
    ReachNode *r;
};

static ReachNode *node_new(double x, CrossSection xs) {
    ReachNode *node;
    NEW(node);
    node->size = 1;
    node->x    = x;
    node->xs   = xs;
    node->l    = NULL;
    node->r    = NULL;
    return node;
}

static void node_free(ReachNode *node) {
    xs_free(node->xs);
    FREE(node);
}

static int node_size(ReachNode *node) {
    if (node == NULL)
        return 0;
    else
        return node->size;
}

static void tree_free(ReachNode *node) {
    if (node) {
        tree_free(node->l);
        tree_free(node->r);
        node_free(node);
    }
}

static CrossSection tree_get(ReachNode *node, double x) {
    if (node == NULL)
        return NULL;

    if (x < node->x)
        return tree_get(node->l, x);
    else if (x > node->x)
        return tree_get(node->r, x);
    else
        return node->xs;
}

static int tree_keys(ReachNode* node, int i, double *x_array) {
    if (node) {
        i = tree_keys(node->l, i, x_array);
        *(x_array + i++) = node->x;
        i = tree_keys(node->r, i, x_array);
    }
    return i;
}

static ReachNode *tree_put(ReachNode* node, double x, CrossSection xs) {
    if (node == NULL)
        return node_new(x, xs);

    if (x < node->x)
        node->l = tree_put(node->l, x, xs);
    else if (x > node->x)
        node->r = tree_put(node->r, x, xs);
    else {
        xs_free(node->xs);
        node->xs = xs;
    }
    node->size = 1 + node_size(node->l) + node_size(node->r);
    return node;
}

struct Reach {
    ReachNode *root;
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
    return node_size(reach->root);
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
}

int reach_stream_distance(Reach reach, double **x) {
    if (!reach || !x)
        RAISE(null_ptr_arg_Error);

    int size = node_size(reach->root);

    *x = Mem_calloc(size, sizeof(double), __FILE__, __LINE__);
    tree_keys(reach->root, 0, *x);

    return size;
}
