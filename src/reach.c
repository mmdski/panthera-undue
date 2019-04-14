#include <cii/mem.h>
#include <panthera/exceptions.h>
#include <panthera/reach.h>
#include <panthera/xstable.h>
#include <math.h>
#include <stddef.h>


typedef struct ReachNode {
    double x;  /* distance downstream */
    double y;  /* thalweg elevation */
    double discharge;
    CrossSection xs;
} ReachNode;


struct ReachNodeProps {
    double *properties;
}

static ReachNodeProps rnp_new(void) {
    ReachNodeProps rnp;
    NEW(rnp);
    rnp->properties = Mem_calloc(N_RNP, sizeof(double), __FILE__, __LINE__);
    return rnp;
}

void rnp_free(ReachNodeProps rnp) {
    if (!rnp)
        RAISE(null_ptr_arg_Error);
    Mem_free(rnp->properties, __FILE__, __LINE__);
    FREE(rnp);
}

static void rnp_set(ReachNodeProps rnp, rn_prop prop, double value) {
    assert(rnp);
    *(rnp->properties + prop) = value;
}

double rnp_get(ReachNodeProps rnp, rn_prop prop) {
    if (!rnp)
        RAISE(null_ptr_arg_Error);
    return *(rnp->properties + prop);
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

    reach->n_nodes   = n_nodes;
    ReachNode *nodes = Mem_calloc(n_nodes, sizeof(ReachNode), __FILE__,
                                  __LINE__);

    nodes->x         = *x;
    nodes->y         = *y;
    nodes->discharge = NAN; /* initialize discharge as NAN */
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

        (nodes + i)->x         = *(x + i);
        (nodes + i)->y         = *(y + i);
        (nodes + i)->discharge = NAN;
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
    if (!reach)
        RAISE(null_ptr_arg_Error);
    Mem_free(reach->nodes, __FILE__, __LINE__);
    FREE(reach);
}

int reach_size(Reach reach) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    return reach->n_nodes;
}
