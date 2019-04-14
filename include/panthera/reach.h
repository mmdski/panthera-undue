#ifndef REACH_INCLUDED
#define REACH_INCLUDED

#include <panthera/crosssection.h>
#include <panthera/xstable.h>

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
 * @RN_WSE:            Water surface elevation of reach node properties
 * @RN_DISCHARGE:      Discharge of reach node properties
 * @RN_VELOCITY:       Mean channel velocity of node
 * @RN_FRICTION_SLOPE: Friction slope of reach node properties
 * @N_RN:              Number of reach node properties
 */
typedef enum {
    RN_X,
    RN_WSE,
    RN_DISCHARGE,
    RN_VELOCITY,
    RN_FRICTION_SLOPE,
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
 * Returns: None
 */
extern void rnp_free(ReachNodeProps rnp);

/**
 * rnp_get:
 * @rnp: a #ReachNodeProps
 * @prop: a #rn_prop
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @rnp is `NULL`
 *
 * Returns: The value of @prop contained in @rnp
 */
extern double rnp_get(ReachNodeProps rnp, rn_prop prop);

/**
 * Reach:
 *
 * Simulation river reach
 */
typedef struct Reach *Reach;

/**
 * reach_new:
 * @n_nodes:    the number of nodes in the reach
 * @x:          x-location of each node
 * @y:          y-location of each node
 * @xs_number:  cross section number of each node
 * @xstable:    a #XSTable containing keys in @xs_number
 *
 * Creates a new #Reach. The returned reach is newly created and should be
 * freed with reach_free() after use.
 *
 * @x contains the distance downstream of each node. The values in @x must be
 * in ascending order.
 *
 * @y contains the elevation of each node. The y-value of each node is
 * is subtracted from elevation to get depth for the computation of cross
 * section properties using xs_hydraulic_properties(). If the lowest y-value
 * in the cross section coordinate array is 0, then @y may be considered the
 * thalweg elevation.
 *
 * @xs_number contains cross section numbers for each node. Cross section
 * numbers must return valid cross sections through xstable_get().
 *
 * @xstable contains cross sections accessible as keys in @xs_number. The
 * returned reach contains references to #CrossSection instances, so @xstable
 * must not be freed while the returned reach is still in use.
 *
 * **Raises:**
 *
 * #value_arg_Error if @n_nodes < 1
 *
 * #null_ptr_arg_Error if @x, @y, @xs_number, or @xstable are `NULL`
 *
 * #reach_xs_num_Error if a cross section number is @xs_number is not contained
 * in @xstable
 *
 * #reach_x_order_Error if the values in @x are not in ascending order
 *
 * Returns: a new reach
 */
extern Reach reach_new(int n_nodes, double *x, double *y, int *xs_number,
                       XSTable xstable);

/**
 * reach_free:
 * @reach: a #Reach
 *
 * Frees a #Reach. The cross sections referenced by @reach are not freed, as
 * they are contained and freed by a cross section table through the
 * [XSTable](xstable.html) interface.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: None
 */
extern void reach_free(Reach reach);

/**
 * reach_size:
 * @reach: a #Reach
 *
 * Returns the number of nodes in @reach.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: size of @reach
 */
extern int reach_size(Reach reach);

/**
 * reach_node_properties:
 * @reach: a #Reach
 * @i:     a node index
 * @wse:   water surface elevation
 * @q:     discharge
 *
 * The returned #ReachNodeProps is newly created and should be freed with
 * rnp_free() after use.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * #index_Error if `i < 0` or `i >= size`, where `size` is the size of @reach
 *
 * Returns: properties of reach node with water surface elevation @wse and
 * discharge @q
 */
extern ReachNodeProps reach_node_properties(Reach reach, int i, double wse,
                                            double q);

#endif
