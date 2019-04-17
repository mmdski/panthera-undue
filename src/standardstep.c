#include <cii/mem.h>
#include <math.h>
#include <panthera/exceptions.h>
#include <panthera/standardstep.h>


struct StandardStepResults {
    int n_nodes;
    double *discharge;
    double *wse;
};

static StandardStepResults ss_res_new(int n_nodes) {
    assert(n_nodes > 0);
    StandardStepResults ssr;
    NEW(ssr);
    ssr->n_nodes   = n_nodes;
    ssr->discharge = Mem_calloc(n_nodes, sizeof(double), __FILE__, __LINE__);
    ssr->wse       = Mem_calloc(n_nodes, sizeof(double), __FILE__, __LINE__);
    return ssr;
}

void ss_res_free(StandardStepResults ssr) {
    if (!ssr)
        RAISE(null_ptr_arg_Error);
    Mem_free(ssr->discharge, __FILE__, __LINE__);
    Mem_free(ssr->wse, __FILE__, __LINE__);
    FREE(ssr);
}

int ss_res_size(StandardStepResults res) {
    if (!res)
        RAISE(null_ptr_arg_Error);
    return res->n_nodes;
}

static void ss_res_set_wse(StandardStepResults ssr, int i, double wse) {
    assert(ssr);
    *(ssr->wse + i)       = wse;
}

double ss_res_get_wse(StandardStepResults res, int i) {
    if (!res)
        RAISE(null_ptr_arg_Error);
    if (i < 0 || res->n_nodes <= i)
        RAISE(index_Error);

    return *(res->wse + i);
}

double ss_res_get_q(StandardStepResults res, int i) {
    if (!res)
        RAISE(null_ptr_arg_Error);
    if (i < 0 || res->n_nodes <= i)
        RAISE(index_Error);

    return *(res->discharge + i);
}

static void ss_res_fill_q(StandardStepResults ssr,
                          StandardStepOptions *options) {
    assert(ssr);
    assert(options);

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

StandardStepResults solve_standard_step(StandardStepOptions *options,
                                        Reach reach) {
    int n_nodes   = reach_size(reach);
    int last_node = n_nodes - 1;
    int last_discharge_node = options->n_discharges - 1;

    if (options->n_discharges < 1 || n_nodes < options->n_discharges)
        RAISE(value_arg_Error);

    /* the last reach in the node must have a discharge */
    if (*(options->discharge_nodes + last_discharge_node) != last_node)
        RAISE(value_arg_Error);

    double eps = 0.003; /* meters */

    int max_iterations = 20;

    int i, j;

    /* water surface elevation */
    double ws1;
    double ws_new;
    double ws_computed;
    double *ws_assumed = Mem_calloc(max_iterations, sizeof(double), __FILE__,
                                    __LINE__);
    double assum_diff;
    double err_diff;
    double *delta = Mem_calloc(max_iterations, sizeof(double), __FILE__,
                               __LINE__);

    /* discharge */
    double q1;
    double q2;

    /* friction slope */
    double sf1;
    double sf2;
    double sf;

    /* distance */
    double x1;
    double x2;
    double dx;

    /* velocity head */
    double vh1;
    double vh2;

    /* head loss */
    double he;

    ReachNodeProps rp1;
    ReachNodeProps rp2;

    /* initialize results and fill it with discharge values */
    StandardStepResults ssr = ss_res_new(n_nodes);
    ss_res_fill_q(ssr, options);

    /* set the first water surface elevation value */
    ss_res_set_wse(ssr, 0, options->boundary_wse);

    for (i = 1; i < n_nodes; i++) {

        /* upstream node */
        ws1 = ss_res_get_wse(ssr, i - 1);
        q1  = ss_res_get_q(ssr, i - 1);
        rp1 = reach_node_properties(reach, i - 1, ws1, q1);
        x1  = rnp_get(rp1, RN_X);
        sf1 = rnp_get(rp1, RN_FRICTION_SLOPE);
        vh1 = rnp_get(rp1, RN_VELOCITY_HEAD);

        q2  = ss_res_get_q(ssr, i);
        for (j = 0; j < max_iterations; j++) {

            if (j == 0)
                ws_new = ws1;
            else if (j == 1)
                ws_new = ws1 + 0.7*(ws_computed - ws1);
            else {
                assum_diff = *(ws_assumed + j - 2) - *(ws_assumed + j - 1);
                err_diff   = *(delta + j - 2) - *(delta + j - 1);
                ws_new = *(ws_assumed + j - 2) - *(delta + j - 2) *
                    assum_diff/err_diff;
            }

            rp2 = reach_node_properties(reach, i, ws_new, q2);
            x2  = rnp_get(rp2, RN_X);
            sf2 = rnp_get(rp2, RN_FRICTION_SLOPE);
            vh2 = rnp_get(rp2, RN_VELOCITY_HEAD);

            sf = 0.5*(sf1 + sf2);
            dx = x2 - x1;
            he = sf*dx;

            ws_computed = ws1 + vh1 - vh2 + he;

            *(delta + j) = fabs(ws_computed - ws_new);
            if (*(delta + j) <= eps) {
                ss_res_set_wse(ssr, i, ws_computed);
                break;
            } else {
                *(ws_assumed + j) = ws_new;
                if (j == max_iterations) {
                    Mem_free(ws_assumed, __FILE__, __LINE__);
                    Mem_free(delta, __FILE__, __LINE__);
                    RAISE(max_iteration_Error);
                }
            }
        }
    }

    Mem_free(ws_assumed, __FILE__, __LINE__);
    Mem_free(delta, __FILE__, __LINE__);

    return ssr;
}
