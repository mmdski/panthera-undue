#include <cii/mem.h>
#include <math.h>
#include <panthera/exceptions.h>
#include <panthera/secantsolve.h>
#include <panthera/standardstep.h>
#include <stdio.h>

#define EPS 0.003
#define MAX_ITERATIONS 20

struct StandardStepResults {
    int     n_nodes;
    double *discharge;
    double *wse;
};

static StandardStepResults
ss_res_new (int n_nodes)
{
    assert (n_nodes > 0);
    StandardStepResults ssr;
    NEW (ssr);
    ssr->n_nodes   = n_nodes;
    ssr->discharge = Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);
    ssr->wse       = Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);
    return ssr;
}

void
ss_res_free (StandardStepResults ssr)
{
    if (!ssr)
        RAISE (null_ptr_arg_error);
    Mem_free (ssr->discharge, __FILE__, __LINE__);
    Mem_free (ssr->wse, __FILE__, __LINE__);
    FREE (ssr);
}

int
ss_res_size (StandardStepResults res)
{
    if (!res)
        RAISE (null_ptr_arg_error);
    return res->n_nodes;
}

static void
ss_res_set_wse (StandardStepResults ssr, int i, double wse)
{
    assert (ssr);
    *(ssr->wse + i) = wse;
}

double
ss_res_get_wse (StandardStepResults res, int i)
{
    if (!res)
        RAISE (null_ptr_arg_error);
    if (i < 0 || res->n_nodes <= i)
        RAISE (index_error);

    return *(res->wse + i);
}

double
ss_res_get_q (StandardStepResults res, int i)
{
    if (!res)
        RAISE (null_ptr_arg_error);
    if (i < 0 || res->n_nodes <= i)
        RAISE (index_error);

    return *(res->discharge + i);
}

static void
ss_res_fill_q (StandardStepResults ssr, StandardStepOptions *options)
{
    assert (ssr);
    assert (options);

    int i, j;

    for (i = 0, j = 0; i < ssr->n_nodes && j < options->n_discharges; i++) {
        *(ssr->discharge + i) = *(options->discharge + j);

        /* advance the discharge node counter if i is equal to the current
         * discharge node
         */
        if (i == *(options->discharge_nodes + j))
            j++;
    }
}

typedef struct NodeSolverData {
    int    i;         /* node index */
    int    direction; /* direction of solution */
    double ws1;       /* previous node wse */
    double q1;        /* previous node discharge */
    double q2;        /* current node discharge */
    Reach  reach;
} NodeSolverData;

double
ws_compute_func (double ws_new, void *function_data)
{
    int   i;
    Reach reach;

    ReachNodeProps rp1;
    double         ws1;
    double         q1;
    double         x1;
    double         sf1;
    double         vh1;

    ReachNodeProps rp2;
    double         q2;
    double         x2;
    double         sf2;
    double         vh2;
    double         ws_computed;

    double sf;
    double dx;
    double he;

    NodeSolverData *solver_data = (NodeSolverData *) function_data;
    i                           = solver_data->i;
    reach                       = solver_data->reach;

    ws1 = solver_data->ws1;
    q1  = solver_data->q1;
    rp1 =
        reach_node_properties (reach, i - 1 * solver_data->direction, ws1, q1);
    x1  = rnp_get (rp1, RN_X);
    sf1 = rnp_get (rp1, RN_FRICTION_SLOPE);
    vh1 = rnp_get (rp1, RN_VELOCITY_HEAD);
    rnp_free (rp1);

    q2  = solver_data->q2;
    rp2 = reach_node_properties (reach, i, ws_new, q2);
    x2  = rnp_get (rp2, RN_X);
    sf2 = rnp_get (rp2, RN_FRICTION_SLOPE);
    vh2 = rnp_get (rp2, RN_VELOCITY_HEAD);
    rnp_free (rp2);

    sf = 0.5 * (sf1 + sf2); /* friction slope */
    dx = x2 - x1;
    he = sf * dx;

    ws_computed = ws1 + vh1 - vh2 - he;

    return ws_computed;
}

StandardStepResults
solve_standard_step (StandardStepOptions *options, Reach reach)
{
    int n_nodes             = reach_size (reach);
    int last_node           = n_nodes - 1;
    int last_discharge_node = options->n_discharges - 1;

    if (!options || !reach)
        RAISE (null_ptr_arg_error);

    if (options->n_discharges < 1 || n_nodes < options->n_discharges)
        RAISE (value_arg_error);

    /* the last reach in the node must have a discharge */
    if (*(options->discharge_nodes + last_discharge_node) != last_node)
        RAISE (value_arg_error);

    if (n_nodes < 2) {
        compute_fail_error.reason = "Reach must have two or more nodes for "
                                    " standard step solution";
        RAISE (compute_fail_error);
    }

    int i, direction, final_compute_node;

    /* reach elevations */
    double *y = Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);
    reach_elevation (reach, y);

    /* water surface elevation */
    double ws1;

    /* discharge */
    double q1;
    double q2;

    SecantSolution *node_solution;

    /* initialize results and fill it with discharge values */
    StandardStepResults ssr = ss_res_new (n_nodes);
    ss_res_fill_q (ssr, options);

    if (options->us_boundary) {
        ss_res_set_wse (ssr, 0, options->boundary_wse);
        i                  = 1;
        direction          = 1;
        final_compute_node = last_node;
    } else {
        ss_res_set_wse (ssr, last_node, options->boundary_wse);
        i                  = last_node - 1;
        direction          = -1;
        final_compute_node = 0;
    }

    /* loop trough nodes */
    for (; direction * i <= final_compute_node; i = i + direction) {
        ws1 = ss_res_get_wse (ssr, i - 1 * direction);
        q1  = ss_res_get_q (ssr, i - 1 * direction);
        q2  = ss_res_get_q (ssr, i);

        NodeSolverData solver_data = { i, direction, ws1, q1, q2, reach };
        node_solution              = secant_solve (
            MAX_ITERATIONS, EPS, &ws_compute_func, &solver_data, ws1);
        if (!(node_solution->solution_found)) {
            RAISE (compute_fail_error);
            FREE (node_solution);
        } else {
            ss_res_set_wse (ssr, i, node_solution->x_computed);
            FREE (node_solution);
        }
    }
    Mem_free (y, __FILE__, __LINE__);

    return ssr;
}
