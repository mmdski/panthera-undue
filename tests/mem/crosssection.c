#include <math.h>
#include <panthera/crosssection.h>
#include <stdlib.h>

void
test_xs_new(void)
{

    int     n           = 5;
    double  z[]         = { 0, 0, 0.5, 1, 1 };
    double  y[]         = { 1, 0, 0, 0, 1 };
    int     n_roughness = 1;
    double  r[]         = { 0.030 };
    double *z_r         = NULL;

    CoArray      ca = coarray_new(n, y, z);
    CrossSection xs = xs_new(ca, n_roughness, r, z_r);
    coarray_free(ca);
    xs_free(xs);
}

void
test_xs_properties(void)
{

    int               n           = 5;
    double            z[]         = { 0, 0, 0.5, 1, 1 };
    double            y[]         = { 1, 0, 0, 0, 1 };
    int               n_roughness = 1;
    double            r[]         = { 0.030 };
    double *          z_r         = NULL;
    CrossSectionProps xsp;

    CoArray      ca = coarray_new(n, y, z);
    CrossSection xs = xs_new(ca, n_roughness, r, z_r);
    coarray_free(ca);

    xsp = xs_hydraulic_properties(xs, 0.5);
    xsp_free(xsp);

    xs_free(xs);
}

void
test_xs_critical_depth(void)
{
    int     i;
    int     n           = 5;
    double  z[]         = { 0, 0, 0.5, 1, 1 };
    double  y[]         = { 1, 0, 0, 0, 1 };
    int     n_roughness = 1;
    double  r[]         = { 0.030 };
    double *z_r         = NULL;

    double depth;
    double critical_flow;
    double critical_depth;

    CoArray           ca = coarray_new(n, y, z);
    CrossSection      xs = xs_new(ca, n_roughness, r, z_r);
    CrossSectionProps xsp;

    for (i = 1; i < 10; i++) {
        depth         = (double) i;
        xsp           = xs_hydraulic_properties(xs, depth);
        critical_flow = xsp_get(xsp, XS_CRITICAL_FLOW);
        xsp_free(xsp);

        critical_depth = xs_critical_depth(xs, critical_flow, 1.25 * depth);
        depth          = critical_depth; /* avoid warnings */
    }

    coarray_free(ca);
    xs_free(xs);
}

void
test_xs_normal_depth(void)
{
    int     i;
    int     n           = 5;
    double  z[]         = { 0, 0, 0.5, 1, 1 };
    double  y[]         = { 1, 0, 0, 0, 1 };
    int     n_roughness = 1;
    double  r[]         = { 0.030 };
    double *z_r         = NULL;

    double depth;
    double normal_flow;
    double normal_depth;
    double slope = 0.001;

    CoArray           ca = coarray_new(n, y, z);
    CrossSection      xs = xs_new(ca, n_roughness, r, z_r);
    CrossSectionProps xsp;

    for (i = 1; i < 10; i++) {
        depth       = (double) i;
        xsp         = xs_hydraulic_properties(xs, depth);
        normal_flow = sqrt(slope) * xsp_get(xsp, XS_CONVEYANCE);
        xsp_free(xsp);

        normal_depth = xs_normal_depth(xs, normal_flow, slope, 1.25 * depth);
        depth        = normal_depth; /* avoid warnings */
    }

    coarray_free(ca);
    xs_free(xs);
}

void
test_crosssection(void)
{
    test_xs_new();
    test_xs_properties();
    test_xs_critical_depth();
    test_xs_normal_depth();
}
