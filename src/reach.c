#include <cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/reach.h>
#include <panthera/xstable.h>
#include <stddef.h>

typedef struct ReachNode {
    double x;  /* distance downstream */
    double y;  /* thalweg elevation */
    CrossSection xs;
} ReachNode;

ReachNode *node_new(double x, double y, CrossSection xs) {
    if (!xs)
        RAISE(null_ptr_arg_Error);
    ReachNode *node;
    NEW(node);
    node->x  = x;
    node->y  = y;
    node->xs = xs;

    return node;
}

void node_free(ReachNode *node) {
    assert(node);
    FREE(node);
}

struct Reach {
    int n_nodes;        /* number of nodes in reach */
    ReachNode *nodes;   /* array of nodes */
};

Reach reach_new(int n_nodes, double *x, double *y, int *xs_number,
                XSTable xstable) {
    if (n_nodes < 1)
        RAISE(value_arg_Error);

    if (!x || !y || !xs_number || !xstable)
        RAISE(null_ptr_arg_Error);

    int i;
    CrossSection xs;

    Reach reach;
    NEW(reach);

    reach->n_nodes = n_nodes;
    ReachNode *nodes   = Mem_calloc(n_nodes, sizeof(ReachNode), __FILE__,
                                    __LINE__);

    nodes->x = *x;
    nodes->y = *y;
    xs = xstable_get(xstable, *xs_number);
    if (xs) {
        nodes->xs = xs;
    }
    else {
        RAISE(reach_xs_num_Error);
    }

    for (i = 1; i < n_nodes; i++) {

        /* make sure the i-th x is greater than the (i-1)-th x */
        if (*(x + i - 1) >= *(x + i))
            RAISE(reach_x_order_Error);

        (nodes + i)->x = *(x + i);
        (nodes + i)->y = *(y + i);
        xs = xstable_get(xstable, *(xs_number + i));
        if (xs)
            (nodes + i)->xs = xs;
        else
            RAISE(reach_xs_num_Error);
    }

    reach->nodes = nodes;

    return reach;
}

void reach_free(Reach reach) {
    Mem_free(reach->nodes, __FILE__, __LINE__);
    FREE(reach);
}
