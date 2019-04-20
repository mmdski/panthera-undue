#include "testlib.h"
#include <glib.h>
#include <panthera/coarray.h>
#include <panthera/standardstep.h>

#define ATOL 1e-3
#define RTOL 0

void
test_normal_flow_us(void)
{

    int i;

    /* cross section */
    int    n_coords = 5;
    double y[]      = {2, 0, 0, 0, 2};
    double z[]      = {0, 0, 0.5, 1, 1};

    double            n_roughness = 1;
    double            roughness[] = {0.03};
    double *          z_roughness = NULL;
    CrossSectionProps xsp;
    double            a;
    double            r;

    /* reach */
    int    n_nodes   = 25;
    int    last_node = n_nodes - 1;
    double slope     = 0.001;
    double dx        = 10;
    double x[n_nodes];
    double y_reach[n_nodes];
    int    xs_number[n_nodes];
    double wse;
    double q;
    double h;

    /* solver options */
    double boundary_depth = 0.75;
    double boundary_wse;
    double discharge;

    CoArray      ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    xsp = xs_hydraulic_properties(xs, boundary_depth);
    a   = xsp_get(xsp, XS_AREA);
    r   = xsp_get(xsp, XS_HYDRAULIC_RADIUS);
    xsp_free(xsp);

    XSTable xstable = xstable_new();
    xstable_put(xstable, 0, xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i - 1) * dx * slope;
        xs_number[i] = 0;
    }

    discharge    = 1 / roughness[0] * a * pow(r, 2. / 3.) * sqrt(slope);
    boundary_wse = boundary_depth + y_reach[0];
    StandardStepOptions options = {
        1, &last_node, &discharge, boundary_wse, true};

    Reach reach = reach_new(n_nodes, x, y_reach, xs_number, xstable);
    StandardStepResults res = solve_standard_step(&options, reach);

    for (i = 0; i < n_nodes; i++) {
        wse = ss_res_get_wse(res, i);
        q   = ss_res_get_q(res, i);
        h   = wse - y_reach[i];
        g_assert_true(test_is_close(q, discharge, ATOL, RTOL));
        g_assert_true(test_is_close(h, boundary_depth, ATOL, RTOL));
    }

    ss_res_free(res);
    reach_free(reach);
    xstable_free(xstable);
}

void
test_normal_flow_ds(void)
{

    int i;

    /* cross section */
    int    n_coords = 5;
    double y[]      = {2, 0, 0, 0, 2};
    double z[]      = {0, 0, 0.5, 1, 1};

    double            n_roughness = 1;
    double            roughness[] = {0.03};
    double *          z_roughness = NULL;
    CrossSectionProps xsp;
    double            a;
    double            r;

    /* reach */
    int    n_nodes   = 25;
    int    last_node = n_nodes - 1;
    double slope     = 0.001;
    double dx        = 10;
    double x[n_nodes];
    double y_reach[n_nodes];
    int    xs_number[n_nodes];
    double wse;
    double q;
    double h;

    /* solver options */
    double boundary_depth = 0.75;
    double boundary_wse;
    double discharge;

    CoArray      ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    xsp = xs_hydraulic_properties(xs, boundary_depth);
    a   = xsp_get(xsp, XS_AREA);
    r   = xsp_get(xsp, XS_HYDRAULIC_RADIUS);
    xsp_free(xsp);

    XSTable xstable = xstable_new();
    xstable_put(xstable, 0, xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i - 1) * dx * slope;
        xs_number[i] = 0;
    }

    discharge    = 1 / roughness[0] * a * pow(r, 2. / 3.) * sqrt(slope);
    boundary_wse = boundary_depth;
    StandardStepOptions options = {
        1, &last_node, &discharge, boundary_wse, false};

    Reach reach = reach_new(n_nodes, x, y_reach, xs_number, xstable);
    StandardStepResults res = solve_standard_step(&options, reach);

    for (i = 0; i < n_nodes; i++) {
        wse = ss_res_get_wse(res, i);
        q   = ss_res_get_q(res, i);
        h   = wse - y_reach[i];
        g_assert_true(test_is_close(q, discharge, ATOL, RTOL));
        g_assert_true(test_is_close(h, boundary_depth, ATOL, RTOL));
    }

    ss_res_free(res);
    reach_free(reach);
    xstable_free(xstable);
}

/* the solution to this problem is table 6-2 of Open-Channnel Flow, 2nd ed. by
 * Chaudhry (2008)
 */
void
test_trapezoid(void)
{
    int i;

    /* cross section */
    int    n_coords = 4;
    double y[]      = {10, 0, 0, 10};
    double z[]      = {0, 20, 30, 50};

    double  n_roughness = 1;
    double  roughness[] = {0.013};
    double *z_roughness = NULL;

    /* reach */
    int     n_nodes   = 5;
    int     last_node = n_nodes - 1;
    double  slope     = 0.001;
    double  dx        = 1000;
    double  x[n_nodes];
    double  y_reach[n_nodes];
    int     xs_number[n_nodes];
    double  wse;
    double *y_elevation =
        Mem_calloc(n_nodes, sizeof(double), __FILE__, __LINE__);

    /* solver options */
    double boundary_depth = 5.0;
    double boundary_wse;
    double discharge;

    double expected_depth[] = {1.263, 2.038, 3.007, 4.002, 5};

    CoArray      ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    XSTable xstable = xstable_new();
    xstable_put(xstable, 0, xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i - 1) * dx * slope;
        xs_number[i] = 0;
    }

    discharge                   = 30;
    boundary_wse                = boundary_depth;
    StandardStepOptions options = {
        1, &last_node, &discharge, boundary_wse, false};

    Reach reach = reach_new(n_nodes, x, y_reach, xs_number, xstable);
    reach_elevation(reach, y_elevation);
    StandardStepResults res = solve_standard_step(&options, reach);

    for (i = 0; i < n_nodes; i++) {
        wse = ss_res_get_wse(res, i);
        g_assert_true(test_is_close(
            wse - y_elevation[i], expected_depth[i], ATOL, RTOL));
    }

    Mem_free(y_elevation, __FILE__, __LINE__);
    ss_res_free(res);
    reach_free(reach);
    xstable_free(xstable);
}

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/panthera/standardstep/normal flow us",
                    test_normal_flow_us);
    g_test_add_func("/panthera/standardstep/normal flow ds",
                    test_normal_flow_ds);
    g_test_add_func("/panthera/standardstep/trapezoid", test_trapezoid);
    return g_test_run();
}
