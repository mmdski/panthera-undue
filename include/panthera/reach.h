#ifndef REACH_INCLUDED
#define REACH_INCLUDED

#include <panthera/crosssection.h>
#include <panthera/reachnode.h>

/**
 * SECTION: reach.h
 * @short_description: River reach
 * @title: Reach
 *
 * Simulation river reach
 */

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
 * reach_rnp:
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
reach_rnp(Reach reach, int i, double wse, double q);

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
