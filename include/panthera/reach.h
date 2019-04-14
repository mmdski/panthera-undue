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
Reach reach_new(int n_nodes, double *x, double *y, int *xs_number,
                XSTable xstable);

/**
 * reach_free:
 * @reach: a #Reach
 *
 * Frees a #Reach. The cross sections referenced by @reach are not freed, as
 * they are contained and freed by a cross section table through the
 * [XSTable](xstable.html) interface.
 *
 * Returns: None
 */
void reach_free(Reach reach);

#endif
