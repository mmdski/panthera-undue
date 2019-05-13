#ifndef STANDARDSTEP_INCLUDED
#define STANDARDSTEP_INCLUDED
#include <panthera/reach.h>
#include <stddef.h>

/**
 * SECTION: standardstep.h
 * @short_description: Standard step solver
 * @title: StandardStep
 *
 * Standard step solver
 */

/**
 * StandardStepOptions:
 * @n_discharges:    number of discharge values to set
 * @discharge_nodes: the nodes of discharge values to set
 * @discharge:       discharge values to set
 * @boundary_wse:    boundary condition water surface elevation
 * @us_boundary:     upstream boundary condition
 *
 * Options for solve_standard_step().
 *
 * If @us_boundary is `true`, the elevation of the `0`-th node in the reach
 * used in the standard step solution will be set to @boundary_wse.
 */
typedef struct {
    int     n_discharges;
    int *   discharge_nodes;
    double *discharge;
    double  boundary_wse;
    bool    us_boundary;
} StandardStepOptions;

/**
 * StandardStepResults:
 *
 * Structure containing results from a standard step solution. Returned from
 * solve_standard_step().
 */
typedef struct StandardStepResults *StandardStepResults;

/**
 * ss_res_free:
 * @ssr: a #StandardStepResults
 *
 * Frees @res
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @res is `NULL`
 *
 * Returns: None
 */
void
ss_res_free (StandardStepResults res);

/**
 * ss_res_size:
 * @res: a #StandardStepResults
 *
 * Returns the number of nodes in the solution @res contains.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @res is `NULL`
 *
 * Returns: the number of nodes in the solution results
 */
int
ss_res_size (StandardStepResults res);

/**
 * ss_res_get_wse:
 * @res: a #StandardStepResults
 * @i:   node index
 *
 * Returns the water surface elevation at @i -th node in @res
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @res is `NULL`
 *
 * #index_error if @i `< 0` or @i `>= size`, where `size` is the
 * number of nodes in @res
 *
 * Returns: the water surface elevation of a simulation results node
 */
double
ss_res_get_wse (StandardStepResults res, int i);

/**
 * ss_res_get_q:
 * @res: a #StandardStepResults
 * @i:   node index
 *
 * Returns the discharge at @i -th node in @res
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @res is `NULL`
 *
 * #index_error if @i `< 0` or @i `>= size`, where `size` is the
 * number of nodes in @res
 *
 * Returns: the discharge of a simulation results node
 */
double
ss_res_get_q (StandardStepResults res, int i);

/**
 * solve_standard_step:
 * @options: a #StandardStepOptions
 * @reach:   a #Reach
 *
 * Computes the standard step steady-state hydraulic solution using the
 * information in @options and the hydraulic geometry in @reach.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @options or @reach is `NULL`
 *
 * #value_arg_error if @options->n_discharges is less than one or greater than
 * the number of nodes in @reach or the last node number in
 * @options->discharge_nodes is not equal to the last node in @reach
 *
 * #compute_fail_error the number of nodes in @reach is less than two or
 *
 * Returns: a steady state hydraulic solution
 */
extern StandardStepResults
solve_standard_step (StandardStepOptions *options, Reach reach);

#endif
