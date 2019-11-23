#ifndef REACH_INCLUDED
#define REACH_INCLUDED

#include <panthera/crosssection.h>

/**
 * SECTION: reach.h
 * @short_description: River reach
 * @title: Reach
 *
 * Simulation river reach
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
 * Reach:
 *
 * Simulation river reach
 */
typedef struct Reach *Reach;

/**
 * reach_new:
 *
 * Creates a new reach. The returned reach is newly created and should be
 * freed with reach_free() after use.
 *
 * Returns: a new reach
 */
extern Reach
reach_new(void);

/**
 * reach_free:
 * @reach: a #Reach
 *
 * Frees @reach. The cross sections referenced by @reach are not freed.
 *
 * Returns: nothing
 */
extern void
reach_free(Reach reach);

/**
 * reach_size:
 * @reach: a #Reach
 *
 * Returns the number of nodes in a reach.
 *
 * Returns: size of @reach
 */
extern int
reach_size(Reach reach);

/**
 * reach_node_properties:
 * @reach: a #Reach
 * @i:     a node index
 * @wse:   water surface elevation
 * @q:     discharge
 *
 * Properties of reach node with water surface elevation @wse and
 * discharge @q.
 *
 * The returned reach node propreties is newly created and should be freed with
 * rnp_free() after use.
 *
 * Returns: reach node properties
 */
extern ReachNodeProps
reach_node_properties(Reach reach, int i, double wse, double q);

/**
 * reach_put_xs:
 * @reach: a #Reach
 * @x:     distance downstream
 * @y:     thalweg elevation
 * xs:     a #CrossSection
 *
 * Create a node in a reach from a cross section.
 *
 * Returns: nothing
 */
extern void
reach_put_xs(Reach reach, double x, double y, CrossSection xs);

/**
 * reach_stream_distance:
 * @reach: a #Reach
 * @x:     an array of doubles
 *
 * Fills @x with the stream distance values of the nodes in @reach. @x must be
 * allocated before being passed as a parameter and should be freed when no
 * longer in use.
 *
 * Returns: nothing
 */
extern void
reach_stream_distance(Reach reach, double *x);

/**
 * reach_elevation:
 * @reach: a #Reach
 * @y:     an array of doubles
 *
 * Fills @y with the elevation values of the nodes in @reach. @y must be
 * allocated before being passed as a parameter and should be freed when no
 * longer in use.
 *
 * Returns: nothing
 */
extern void
reach_elevation(Reach reach, double *y);

#endif
