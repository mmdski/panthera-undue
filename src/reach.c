#include "mem.h"
#include "redblackbst.h"
#include <assert.h>
#include <math.h>
#include <panthera/constants.h>
#include <panthera/reach.h>
#include <stddef.h>

struct ReachNodeProps {
    double *properties;
};

static ReachNodeProps
rnp_new(void)
{
    ReachNodeProps rnp;
    NEW(rnp);
    rnp->properties = mem_calloc(N_RN, sizeof(double), __FILE__, __LINE__);
    return rnp;
}

void
rnp_free(ReachNodeProps rnp)
{
    assert(rnp);
    mem_free(rnp->properties, __FILE__, __LINE__);
    FREE(rnp);
}

static void
rnp_set(ReachNodeProps rnp, rn_prop prop, double value)
{
    assert(rnp);
    *(rnp->properties + prop) = value;
}

double
rnp_get(ReachNodeProps rnp, rn_prop prop)
{
    assert(rnp);
    return *(rnp->properties + prop);
}

struct ReachNode {
    double       x; /* distance downstream */
    double       y; /* thalweg elevation */
    CrossSection xs;
};

typedef struct ReachNode *ReachNode;

struct Reach {
    ReachNode * nodes; /* array of nodes */
    RedBlackBST tree;  /* reach node tree */
};

int
key_compare_func(const void *x, const void *y)
{
    double x_key = *(double *) x;
    double y_key = *(double *) y;

    if (x_key < y_key)
        return -1;
    else if (x_key > y_key)
        return 1;
    else
        return 0;
}

Reach
reach_new(void)
{
    Reach reach;
    NEW(reach);

    reach->nodes = NULL;
    reach->tree  = redblackbst_new(&key_compare_func);

    return reach;
}

void
reach_free(Reach reach)
{
    assert(reach);

    int         n;
    RedBlackBST tree = reach->tree;
    n                = redblackbst_size(tree);

    if (n > 0) {

        void **tree_keys = mem_calloc(n, sizeof(void *), __FILE__, __LINE__);

        Item *item;

        redblackbst_keys(tree, tree_keys);

        for (int i = 0; i < n; i++) {
            item = redblackbst_get(tree, tree_keys[i]);
            redblackbst_delete(tree, tree_keys[i]);
            FREE(item->key);
            FREE(item->value);
            redblackbst_free_item(item);
        }
        mem_free(tree_keys, __FILE__, __LINE__);
    }

    redblackbst_free(tree);

    if (reach->nodes)
        mem_free(reach->nodes, __FILE__, __LINE__);

    FREE(reach);
}

static void
create_array(Reach reach)
{
    int n = redblackbst_size(reach->tree);

    if (n == 0)
        return;

    void **     keys  = mem_calloc(n, sizeof(void *), __FILE__, __LINE__);
    ReachNode * nodes = mem_calloc(n, sizeof(ReachNode *), __FILE__, __LINE__);
    Item *      item;
    RedBlackBST tree;

    tree = reach->tree;

    redblackbst_keys(tree, keys);

    for (int i = 0; i < n; i++) {
        item     = redblackbst_get(tree, keys[i]);
        nodes[i] = (ReachNode) item->value;
        redblackbst_free_item(item);
        item = NULL;
    }

    reach->nodes = nodes;

    mem_free(keys, __FILE__, __LINE__);
}

static void
free_array(Reach reach)
{
    if (reach->nodes)
        mem_free(reach->nodes, __FILE__, __LINE__);
    reach->nodes = NULL;
}

int
reach_size(Reach reach)
{
    assert(reach);
    return redblackbst_size(reach->tree);
}

void
reach_stream_distance(Reach reach, double *x)
{
    assert(reach && x);

    if (reach->nodes == NULL)
        create_array(reach);

    int       i;
    int       n = redblackbst_size(reach->tree);
    ReachNode node;

    for (i = 0; i < n; i++) {
        node     = *(reach->nodes + i);
        *(x + i) = node->x;
    }
}

void
reach_elevation(Reach reach, double *y)
{
    assert(reach && y);

    if (reach->nodes == NULL)
        create_array(reach);

    int       i;
    int       n = redblackbst_size(reach->tree);
    ReachNode node;

    for (i = 0; i < n; i++) {
        node     = *(reach->nodes + i);
        *(y + i) = node->y;
    }
}

ReachNodeProps
reach_node_properties(Reach reach, int i, double wse, double q)
{
    assert(reach);
    assert(0 <= i && i < redblackbst_size(reach->tree));

    if (reach->nodes == NULL)
        create_array(reach);

    ReachNode node = *(reach->nodes + i);

    double h = wse - node->y;

    CrossSectionProps xsp            = xs_hydraulic_properties(node->xs, h);
    double            area           = xsp_get(xsp, XS_AREA);
    double            conveyance     = xsp_get(xsp, XS_CONVEYANCE);
    double            velocity_coeff = xsp_get(xsp, XS_VELOCITY_COEFF);
    xsp_free(xsp);

    double velocity       = q / area;
    double friction_slope = (q * q) / (conveyance * conveyance);
    double velocity_head =
        velocity_coeff * velocity * velocity / (2 * GRAVITY);

    ReachNodeProps rnp = rnp_new();
    rnp_set(rnp, RN_X, node->x);
    rnp_set(rnp, RN_Y, node->y);
    rnp_set(rnp, RN_WSE, wse);
    rnp_set(rnp, RN_DISCHARGE, q);
    rnp_set(rnp, RN_VELOCITY, velocity);
    rnp_set(rnp, RN_FRICTION_SLOPE, friction_slope);
    rnp_set(rnp, RN_VELOCITY_HEAD, velocity_head);

    return rnp;
}

void
reach_put(Reach reach, double x, double y, CrossSection xs)
{
    assert(reach && xs);

    double *tree_key;

    ReachNode node;
    NEW(node);

    node->x  = x;
    node->y  = y;
    node->xs = xs;

    Item *item = redblackbst_get(reach->tree, &x);
    if (item) {
        tree_key = item->key;
        FREE(item->value);
        redblackbst_free_item(item);
    } else {
        tree_key  = mem_alloc(sizeof(double), __FILE__, __LINE__);
        *tree_key = x;
    }

    redblackbst_put(reach->tree, tree_key, node);
    free_array(reach);
}
