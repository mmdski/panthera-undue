#include "memtest.h"

void
test_standardstep (void)
{

    int i;

    /* cross section */
    int    n_coords = 5;
    double y[]      = { 2, 0, 0, 0, 2 };
    double z[]      = { 0, 0, 0.5, 1, 1 };

    double            n_roughness = 1;
    double            roughness[] = { 0.03 };
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

    /* solver options */
    double boundary_depth = 0.75;
    double boundary_wse;
    double discharge;

    CoArray      ca = coarray_new (n_coords, y, z);
    CrossSection xs = xs_new (ca, n_roughness, roughness, z_roughness);
    coarray_free (ca);

    xsp = xs_hydraulic_properties (xs, boundary_depth);
    a   = xsp_get (xsp, XS_AREA);
    r   = xsp_get (xsp, XS_HYDRAULIC_RADIUS);
    xsp_free (xsp);

    XSTable xstable = xstable_new ();
    xstable_put (xstable, 0, xs);
    xs_free (xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i - 1) * dx * slope;
        xs_number[i] = 0;
    }

    discharge    = 1 / roughness[0] * a * pow (r, 2. / 3.) * sqrt (slope);
    boundary_wse = boundary_depth + y_reach[0];
    StandardStepOptions options = {
        1, &last_node, &discharge, boundary_wse, true /* upstream boundary */
    };

    Reach reach = reach_new (n_nodes, x, y_reach, xs_number, xstable);
    StandardStepResults res = solve_standard_step (&options, reach);

    ss_res_free (res);
    reach_free (reach);
    xstable_free (xstable);
}
