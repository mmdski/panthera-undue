#include <cii/mem.h>
#include <math.h>
#include <panthera/coarray.h>
#include <panthera/standardstep.h>
#include <stdio.h>

int main (void) {

    int i;

    /* cross section */
    int n_coords = 5;
    double y[]   = {1, 0, 0, 0, 1};
    double z[]   = {0, 0, 0.5, 1, 1};

    double n_roughness      = 1;
    double roughness[]      = {0.03};
    double *z_roughness     = NULL;
    CrossSectionProps xsp;
    double a;
    double r;

    /* reach */
    int n_nodes     = 25;
    int last_node   = n_nodes - 1;
    double slope    = 0.001;
    double dx       = 10;
    double x[n_nodes];
    double y_reach[n_nodes];
    int xs_number[n_nodes];

    /* solver options */
    double boundary_wse = 0.75;
    double discharge;

    CoArray ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    xsp = xs_hydraulic_properties(xs, boundary_wse);
    a = xsp_get(xsp, XS_AREA);
    r = xsp_get(xsp, XS_HYDRAULIC_RADIUS);
    xsp_free(xsp);

    discharge = 1/roughness[0] * a * pow(r, 2./3.) * sqrt(slope);
    StandardStepOptions options = {
        1,
        &last_node,
        &discharge,
        boundary_wse
    };

    XSTable xstable = xstable_new();
    xstable_put(xstable, 0, xs);

    for (i = 0; i < n_nodes; i++) {
        x[i]         = i * dx;
        y_reach[i]   = (n_nodes - i) * dx * slope;
        xs_number[i] = 0;
    }

    Reach reach = reach_new(n_nodes, x, y_reach, xs_number, xstable);

    StandardStepResults res = solve_standard_step(&options, reach);

    ss_res_free(res);
    reach_free(reach);
    xstable_free(xstable);
}
