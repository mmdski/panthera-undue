#include <cii/mem.h>
#include <panthera/constants.h>
#include <panthera/exceptions.h>
#include <panthera/reach.h>
#include <panthera/xstable.h>
#include <math.h>
#include <stddef.h>


struct ReachNodeProps {
    double *properties;
};

static ReachNodeProps rnp_new(void) {
    ReachNodeProps rnp;
    NEW(rnp);
    rnp->properties = Mem_calloc(N_RN, sizeof(double), __FILE__, __LINE__);
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

typedef struct ReachNode {
    double x;  /* distance downstream */
    double y;  /* thalweg elevation */
    CrossSection xs;
} ReachNode;

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

ReachNodeProps reach_node_properties(Reach reach, int i, double wse,
                                     double q) {
    if (!reach)
        RAISE(null_ptr_arg_Error);
    if (i < 0 || reach->n_nodes <= i)
        RAISE(index_Error);

    ReachNode *node = (reach->nodes + i);

    double h = wse - node->y;

    CrossSectionProps xsp = xs_hydraulic_properties(node->xs, h);
    double area           = xsp_get(xsp, XS_AREA);
    double conveyance     = xsp_get(xsp, XS_CONVEYANCE);
    double velocity_coeff = xsp_get(xsp, XS_VELOCITY_COEFF);
    xsp_free(xsp);

    double velocity       = q / area;
    double friction_slope = q / conveyance;
    double velocity_head  = velocity_coeff * velocity * velocity
                                / (2 * GRAVITY);

    ReachNodeProps rnp = rnp_new();
    rnp_set(rnp, RN_X,              node->x);
    rnp_set(rnp, RN_WSE,            wse);
    rnp_set(rnp, RN_DISCHARGE,      q);
    rnp_set(rnp, RN_VELOCITY,       velocity);
    rnp_set(rnp, RN_FRICTION_SLOPE, friction_slope);
    rnp_set(rnp, RN_VELOCITY_HEAD,  velocity_head);

    return rnp;
}
