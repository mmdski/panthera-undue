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
    node->color = RED;
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

static TreeNode *tree_min(TreeNode *node) {
    if (node->l == NULL)
        return node;
    else
        return tree_min(node);
}

static TreeNode *tree_get(TreeNode *node, double x) {
    if (node == NULL)
        return NULL;

    if (x < node->x)
        return tree_get(node->l, x);
    else if (x > node->x)
        return tree_get(node->r, x);
    else
        return node;
}

static bool tree_contains(TreeNode *node, double x) {
    return tree_get(node, x) != NULL;
}

static int tree_is_red(TreeNode *node) {
    return node != NULL && node->color;
}

static TreeNode *tree_rotate_right(TreeNode *h) {
    assert(h);
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
    assert(h);
    assert(h->r->color == RED);

    TreeNode *x = h->r;
    h->r = x->l;
    x->l = h;
    x->color = x->l->color;
    x->l->color = RED;
    x->size = h->size;
    h->size = tree_size(h->l) + tree_size(h->r) + 1;
    return x;
}

/* flip the colors of a node and its two children */
static void tree_flip_colors(TreeNode *node) {
    assert(node && node->l && node->r);
    node->color = !(node->color);
    node->l->color = !(node->l->color);
    node->r->color = !(node->r->color);
}

/* restore red-black tree invariant */
static TreeNode *tree_balance(TreeNode *node) {
    assert(node);

    if (tree_is_red(node->r))
        node = tree_rotate_left(node);
    if (tree_is_red(node->l) && tree_is_red(node->l->l))
        node = tree_rotate_right(node);
    if (tree_is_red(node->l) && tree_is_red(node->r))
        tree_flip_colors(node);

    node->size = tree_size(node->l) + tree_size(node->r) + 1;
    return node;
}

/* assuming that node is red and both node->left and node->left->left are
 * black, make node->left or one of its children red.
 */
static TreeNode *tree_move_red_left(TreeNode *node) {
    assert(node);
    assert(tree_is_red(node) && !tree_is_red(node->l) &&
           !tree_is_red(node->l->l));

    tree_flip_colors(node);
    if (tree_is_red(node->r->l)) {
        node->r = tree_rotate_right(node->r);
        node = tree_rotate_left(node);
        tree_flip_colors(node);
    }
    return node;
}

/* assuming that node is red and both node->right and node->right->left are
 * black, make node->right or one of its children red.
 */
static TreeNode *tree_move_red_right(TreeNode *node) {
    assert(node);
    assert(tree_is_red(node) && !tree_is_red(node->r) &&
           !tree_is_red(node->r->l));

    tree_flip_colors(node);
    if (tree_is_red(node->l->l)) {
        node = tree_rotate_right(node);
        tree_flip_colors(node);
    }
    return node;
}

/* delete the node with the minimum x rooted at node */
static TreeNode *tree_delete_min(TreeNode *node) {
    if (node->l == NULL) {
        tree_node_free(node);
        return NULL;
    }

    if (!tree_is_red(node->l) && !tree_is_red(node->l->l))
        node = tree_move_red_left(node);

    node->l = tree_delete_min(node->l);
    return tree_balance(node);
}

/* delete the node with the given key rooted at node */
static TreeNode *tree_delete(TreeNode *node, double x) {
    assert(tree_get(node, x) != NULL);

    if (x < node->x) {
        if (!tree_is_red(node->l) && !tree_is_red(node->l->l))
            node = tree_move_red_left(node);
        node->l = tree_delete(node->l, x);
    } else {
        if(tree_is_red(node->l))
            node = tree_rotate_right(node);
        if (x == node->x && node->r == NULL) {
            tree_node_free(node);
            return NULL;
        }
        if (!tree_is_red(node->r) && !tree_is_red(node->r->l))
            node = tree_move_red_right(node);
        if (x == node->x) {
            TreeNode *n = tree_min(node->r);
            node->x = n->x;
            node->xs = n->xs;
            node->r = tree_delete_min(node->r);
        } else
            node->r = tree_delete(node->r, x);
    }
    return tree_balance(node);
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

double reach_min_x(Reach reach) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    if (reach_size(reach) == 0)
        RAISE(empty_reach_Error);

    TreeNode *min = tree_min(reach->root);
    return min->x;
}

CrossSection reach_get(Reach reach, double x) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    TreeNode *node = tree_get(reach->root, x);
    if (node)
        return node->xs;
    else
        return NULL;
}

void reach_put(Reach reach, double x, CrossSection xs) {
    if (!reach || !xs)
        RAISE(null_ptr_arg_Error);

    reach->root = tree_put(reach->root, x, xs);
    reach->root->color = BLACK;
}

void reach_delete(Reach reach, double x) {
    if (!tree_contains(reach->root, x))
        return;

    /* if both children of root are black, set root to red */
    if (!tree_is_red(reach->root->l) && !tree_is_red(reach->root->r))
        reach->root->color = RED;

    reach->root = tree_delete(reach->root, x);
    if (reach_size(reach) == 0)
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
