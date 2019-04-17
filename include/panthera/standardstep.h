#ifndef STANDARDSTEP_INCLUDED
#define STANDARDSTEP_INCLUDED
#include <panthera/reach.h>
#include <stddef.h>

/**
 * SECTION: standardstep.h
 * @short_description: Standard step solver
 * @title: Standard step
 *
 * Standard step solver
 */

/**
 * StandardStepOptions:
 * @n_discharges:    number of discharge values to set
 * @discharge_nodes: the nodes of discharge values to set
 * @discharge:       discharge values to set
 * @boundary_wse:    boundary condition water surface elevation
 *
 * Options for solve_standard_step().
 *
 * The elevation of the `0`-th node in the reach used in the standard step
 * solution will be set to @boundary_wse.
 */
typedef struct {
    int     n_discharges;
    int    *discharge_nodes;
    double *discharge;
    double  boundary_wse;
} StandardStepOptions;

typedef struct StandardStepResults *StandardStepResults;

void ss_res_free(StandardStepResults ssr);

int ss_res_size(StandardStepResults res);

double ss_res_get_wse(StandardStepResults res, int i);

double ss_res_get_q(StandardStepResults res, int i);

extern StandardStepResults solve_standard_step(StandardStepOptions *options,
                                               Reach reach);

#endif
