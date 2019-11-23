#ifndef REACHNODE_INCLUDED
#define REACHNODE_INCLUDED

#include <panthera/crosssection.h>

/**
 * SECTION: reachnode.h
 * @short_description: Reach node
 * @title: ReachNode
 *
 * Reach node
 */

/**
 * rn_prop:
 * @RN_X:              Downstream distance of reach node
 * @RN_Y:              Elevation of reach node
 * @RN_WSE:            Water surface elevation of reach node properties
 * @RN_DISCHARGE:      Discharge of reach node properties
 * @RN_VELOCITY:       Mean channel velocity of node
 * @RN_FRICTION_SLOPE: Friction slope of reach node properties
 * @N_RN:              Number of reach node properties
 */
typedef enum {
    RN_X,
    RN_Y,
    RN_WSE,
    RN_DISCHARGE,
    RN_VELOCITY,
    RN_FRICTION_SLOPE,
    RN_VELOCITY_HEAD,
    N_RN
} rn_prop;

/**
 * ReachNodeProps:
 *
 * Reach node properties
 *
 */
typedef struct ReachNodeProps *ReachNodeProps;

/**
 * rnp_free:
 * @rnp: a #ReachNodeProps
 *
 * Frees @rnp
 *
 * Returns: nothing
 */
extern void
rnp_free(ReachNodeProps rnp);

/**
 * rnp_get:
 * @rnp: a #ReachNodeProps
 * @prop: a #rn_prop
 *
 * Returns: a reach node property value
 */
extern double
rnp_get(ReachNodeProps rnp, rn_prop prop);

/**
 * ReachNode:
 *
 * Reach node
 */
typedef struct ReachNode *ReachNode;

/**
 * reachnode_new:
 * @x: longitudinal distance downstream
 * @y: thalweg elevation
 * @xs: a #CrossSection
 *
 * Creates a new reach node. The returned reach node is newly created and
 * should be freed after use.
 *
 * Returns: a new #ReachNode
 */
extern ReachNode
reachnode_new(double x, double y, CrossSection xs);

/**
 * reachnode_free:
 * @node: a #ReachNode
 *
 * Frees a reach node.
 *
 * Returns: nothing
 */
extern void
reachnode_free(ReachNode node);

/**
 * reachnode_x:
 * @node: a #ReachNode
 *
 * Returns: the longitudinal coordinate of a reach node
 */
extern double
reachnode_x(ReachNode node);

/**
 * reachnode_y:
 * @node: a #ReachNode
 *
 * Returns: the thalweg elevation of a reach node
 */
extern double
reachnode_y(ReachNode node);

/**
 * reachnode_xsp:
 * @node: a #ReachNode
 * @wse:  water surface elevation
 *
 * Returns the cross section properties of the cross section contained in a
 * reach node. The returned cross section properties are newly created and
 * should be freed when no longer in use.
 *
 * Returns: cross section properties
 */
extern CrossSectionProps
reachnode_xsp(ReachNode node, double wse);

/**
 * reachnode_properties:
 * @node: a #ReachNode
 * @wse: water surface elevation
 * @q: discharge
 *
 * Returns properties for a reach node. The returned reach node properties is
 * newly created and must be freed when no longer in use.
 *
 * Returns: properties for a reach node
 */
extern ReachNodeProps
reachnode_properties(ReachNode node, double wse, double q);

#endif
