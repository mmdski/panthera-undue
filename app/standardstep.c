#include <cii/mem.h>
#include <math.h>
#include <panthera/coarray.h>
#include <panthera/standardstep.h>
#include <stdio.h>

#define N_NODES 5

int
main (void)
{
    int i;

    /* cross section */
    int    n_coords = 4;
    double y[]      = { 10, 0, 0, 10 };
    double z[]      = { 0, 20, 30, 50 };

    double            n_roughness = 1;
    double            roughness[] = { 0.013 };
    double *          z_roughness = NULL;
    CrossSectionProps xsp;

    /* reach */
    int     n_nodes   = N_NODES;
    int     last_node = n_nodes - 1;
    double  slope     = 0.001;
    double  dx        = 1000;
    double  x[N_NODES];
    double  y_reach[N_NODES];
    int     xs_number[N_NODES];
    double  wse;
    double  q;
    double *y_elevation =
        Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);

    /* solver options */
    double boundary_depth = 5.0;
    double boundary_wse;
    double discharge;

    CoArray      ca = coarray_new (n_coords, y, z);
    CrossSection xs = xs_new (ca, n_roughness, roughness, z_roughness);
    coarray_free (ca);

    XSTable xstable = xstable_new ();
    xstable_put (xstable, 0, xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i - 1) * dx * slope;
        xs_number[i] = 0;
    }

    discharge                   = 30;
    boundary_wse                = boundary_depth;
    StandardStepOptions options = {
        1, &last_node, &discharge, boundary_wse, false
    };

    Reach reach = reach_new (n_nodes, x, y_reach, xs_number, xstable);
    reach_elevation (reach, y_elevation);
    StandardStepResults res = solve_standard_step (&options, reach);

    printf ("Slope      = %f\n", slope);
    printf ("Downstream h = %01.2f\n", boundary_depth);
    printf ("Discharge  = %f\n", discharge);
    printf ("%10s%10s%10s%10s%10s%10s\n", "x", "y", "wse", "h", "Q", "A");
    for (i = 0; i < n_nodes; i++) {
        wse = ss_res_get_wse (res, i);
        q   = ss_res_get_q (res, i);
        xsp = xs_hydraulic_properties (xs, wse - y_elevation[i]);
        printf ("%10.0f%10.2f%10.5f%10.5f%10.4f%10.4f\n",
                x[i],
                y_elevation[i],
                wse,
                wse - y_elevation[i],
                q,
                xsp_get (xsp, XS_AREA));
        xsp_free (xsp);
    }

    Mem_free (y_elevation, __FILE__, __LINE__);
    ss_res_free (res);
    reach_free (reach);
    xstable_free (xstable);
}
