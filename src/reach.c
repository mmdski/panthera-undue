#include <cii/mem.h>
#include <panthera/reach.h>

typedef struct TreeNode TreeNode;

enum color {BLACK, RED};

struct TreeNode {
    double key;
    CrossSection value;

    int size;
    bool color; /* red or black */

    TreeNode *l; /* left */
    TreeNode *r; /* right */
};

static TreeNode *tree_node_new(double key, CrossSection value) {
    TreeNode *node;
    NEW(node);
    node->size  = 1;
    node->color = RED;
    node->key   = key;
    node->value = value;
    node->l     = NULL;
    node->r     = NULL;
    return node;
}

static void tree_node_free(TreeNode *node) {
    if (node->value)
        xs_free(node->value);
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
        return tree_min(node->l);
}

static TreeNode *tree_max(TreeNode *node) {
    if (node->r == NULL)
        return node;
    else
        return tree_max(node->r);
}

static TreeNode *tree_get(TreeNode *node, double key) {
    if (node == NULL)
        return NULL;

    if (key < node->key)
        return tree_get(node->l, key);
    else if (key > node->key)
        return tree_get(node->r, key);
    else
        return node;
}

static bool tree_contains(TreeNode *node, double key) {
    return tree_get(node, key) != NULL;
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
static TreeNode *tree_delete(TreeNode *node, double key) {
    assert(tree_get(node, key) != NULL);

    if (key < node->key) {
        if (!tree_is_red(node->l) && !tree_is_red(node->l->l))
            node = tree_move_red_left(node);
        node->l = tree_delete(node->l, key);
    } else {
        if(tree_is_red(node->l))
            node = tree_rotate_right(node);
        if (key == node->key && node->r == NULL) {
            tree_node_free(node);
            return NULL;
        }
        if (!tree_is_red(node->r) && !tree_is_red(node->r->l))
            node = tree_move_red_right(node);
        if (key == node->key) {
            TreeNode *min_r = tree_min(node->r);
            node->key   = min_r->key;
            xs_free(node->value);
            node->value = min_r->value;
            min_r->value = NULL;
            node->r = tree_delete_min(node->r);
        } else
            node->r = tree_delete(node->r, key);
    }
    return tree_balance(node);
}

static int tree_keys(TreeNode* node, int i, double *key_array) {
    if (node) {
        i = tree_keys(node->l, i, key_array);
        *(key_array + i++) = node->key;
        i = tree_keys(node->r, i, key_array);
    }
    return i;
}

static TreeNode *tree_put(TreeNode* node, double key, CrossSection value) {
    if (!node)
        return tree_node_new(key, value);

    if (key < node->key)
        node->l = tree_put(node->l, key, value);
    else if (key > node->key)
        node->r = tree_put(node->r, key, value);
    else {
        xs_free(node->value);
        node->value = value;
    }

    /* fix any right-leaning links */
    if (tree_is_red(node->r) && !tree_is_red(node->l))
        node = tree_rotate_left(node);
    if (tree_is_red(node->l) && tree_is_red(node->l->l))
        node = tree_rotate_right(node);
    if (tree_is_red(node->l) && (tree_is_red(node->r)))
        tree_flip_colors(node);
    node->size = 1 + tree_size(node->l) + tree_size(node->r);
    return node;
}

struct ReachNode {
    TreeNode *node;
};

static ReachNode reach_node_new(void) {
    ReachNode node;
    NEW(node);
    return node;
}

void reach_node_free(ReachNode node) {
    if (!node)
        RAISE(null_ptr_arg_Error);
    FREE(node);
}

double reach_node_x(ReachNode node) {
    if (!node)
        RAISE(null_ptr_arg_Error);
    return node->node->key;
}

CrossSection reach_node_xs(ReachNode node) {
    if (!node)
        RAISE(null_ptr_arg_Error);
    return node->node->value;
}

struct Reach {
    TreeNode *root;
    ReachNode *reach_node_array;
    int n_reach_nodes;
};

Reach reach_new(void) {
    Reach reach;
    NEW(reach);

    reach->root = NULL;
    reach->reach_node_array = NULL;
    reach->n_reach_nodes = 0;

    return reach;
}

static int fill_node_array(TreeNode *node, int i, ReachNode *node_array) {
    if (node) {
        i = fill_node_array(node->l, i, node_array);
        ReachNode rn = reach_node_new();
        rn->node = node;
        *(node_array + i++) = rn;
        i = fill_node_array(node->r, i, node_array);
    }
    return i;
}

static void init_node_array(Reach reach) {
    int n = reach_size(reach);
    reach->n_reach_nodes = n;
    reach->reach_node_array = Mem_calloc(n, sizeof(ReachNode), __FILE__,
                                         __LINE__);
    fill_node_array(reach->root, 0, reach->reach_node_array);
}

static void free_node_array(Reach reach) {
    int i;
    int n = reach->n_reach_nodes;

    for (i = 0; i < n; i++) {
        reach_node_free(*(reach->reach_node_array + i));
    }

    Mem_free(reach->reach_node_array, __FILE__, __LINE__);
    reach->reach_node_array = NULL;
    reach->n_reach_nodes = 0;
}

void reach_free(Reach reach) {
    tree_free(reach->root);
    if (reach->reach_node_array)
        free_node_array(reach);
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
    return min->key;
}

double reach_max_x(Reach reach) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    if (reach_size(reach) == 0)
        RAISE(empty_reach_Error);

    TreeNode *max = tree_max(reach->root);
    return max->key;
}

CrossSection reach_get(Reach reach, double x) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    TreeNode *node = tree_get(reach->root, x);
    if (node)
        return node->value;
    else
        return NULL;
}

ReachNode reach_get_index(Reach reach, int i) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    if (i > (reach_size(reach) - 1))
        RAISE(index_Error);
    if (!reach->reach_node_array)
        init_node_array(reach);
    return *(reach->reach_node_array + i);
}

void reach_put(Reach reach, double x, CrossSection xs) {
    if (!reach || !xs)
        RAISE(null_ptr_arg_Error);

    reach->root = tree_put(reach->root, x, xs);
    reach->root->color = BLACK;
    if (reach->reach_node_array)
        free_node_array(reach);
}

bool reach_contains(Reach reach, double x) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    return tree_contains(reach->root, x);
}

void reach_delete(Reach reach, double x) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    if (!tree_contains(reach->root, x))
        return;

    /* if both children of root are black, set root to red */
    if (!tree_is_red(reach->root->l) && !tree_is_red(reach->root->r))
        reach->root->color = RED;

    reach->root = tree_delete(reach->root, x);
    if (reach_size(reach) > 0)
        reach->root->color = BLACK;

    if (reach->reach_node_array)
        free_node_array(reach);
}

int reach_stream_distance(Reach reach, double **x) {
    if (!reach || !x)
        RAISE(null_ptr_arg_Error);

    int size = tree_size(reach->root);

    *x = Mem_calloc(size, sizeof(double), __FILE__, __LINE__);
    tree_keys(reach->root, 0, *x);

    return size;
}
