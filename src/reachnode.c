#include "mem.h"
#include <assert.h>
#include <panthera/constants.h>
#include <panthera/reachnode.h>

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

ReachNode
reachnode_new(double x, double y, CrossSection xs)
{

    assert(xs);

    ReachNode node;
    NEW(node);

    node->x  = x;
    node->y  = y;
    node->xs = xs;

    return node;
}

void
reachnode_free(ReachNode node)
{
    assert(node);
    FREE(node);
}

double
reachnode_x(ReachNode node)
{
    assert(node);
    return node->x;
}

double
reachnode_y(ReachNode node)
{
    assert(node);
    return node->y;
}

CrossSectionProps
reachnode_xsp(ReachNode node, double y)
{
    assert(node);
    double h = y - node->y;
    return xs_hydraulic_properties(node->xs, h);
}

ReachNodeProps
reachnode_properties(ReachNode node, double wse, double q)
{
    assert(node);

    CrossSectionProps xsp            = reachnode_xsp(node, wse);
    double            area           = xsp_get(xsp, XS_AREA);
    double            conveyance     = xsp_get(xsp, XS_CONVEYANCE);
    double            velocity_coeff = xsp_get(xsp, XS_VELOCITY_COEFF);
    xsp_free(xsp);

    double velocity       = q / area;
    double friction_slope = (q * q) / (conveyance * conveyance);
    double velocity_head =
        velocity_coeff * velocity * velocity / (2 * GRAVITY);

    ReachNodeProps rnp = rnp_new();
    rnp_set(rnp, RN_X, reachnode_x(node));
    rnp_set(rnp, RN_Y, reachnode_y(node));
    rnp_set(rnp, RN_WSE, wse);
    rnp_set(rnp, RN_DISCHARGE, q);
    rnp_set(rnp, RN_VELOCITY, velocity);
    rnp_set(rnp, RN_FRICTION_SLOPE, friction_slope);
    rnp_set(rnp, RN_VELOCITY_HEAD, velocity_head);

    return rnp;
}
