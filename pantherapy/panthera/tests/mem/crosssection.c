#include "memtest.h"

void
test_xs_roughness (void)
{
    int     n           = 5;
    double  z[]         = { 0, 0, 0.5, 1, 1 };
    double  y[]         = { 1, 0, 0, 0, 1 };
    int     n_roughness = 1;
    double  r[]         = { 0.030 };
    double *z_r         = NULL;

    int     n_roughness_test;
    double *r_test;
    double *z_r_test = NULL;

    CoArray      ca = coarray_new (n, y, z);
    CrossSection xs = xs_new (ca, n_roughness, r, z_r);

    n_roughness_test = xs_n_subsections (xs);

    r_test =
        Mem_calloc (n_roughness_test, sizeof (double), __FILE__, __LINE__);
    xs_roughness (xs, r_test);

    xs_z_roughness (xs, z_r_test);

    xs_free (xs);
    coarray_free (ca);
    Mem_free (r_test, __FILE__, __LINE__);
}

void
test_xs_n_roughness (void)
{

    int    n           = 5;
    double z[]         = { 0, 0, 0.5, 1, 1 };
    double y[]         = { 1, 0, 0, 0, 1 };
    int    n_roughness = 3;
    double r[]         = { 0.030, 0.015, 0.030 };
    double z_r[]       = { 0.25, 0.75 };

    int     n_roughness_test;
    double *r_test;
    double *z_r_test;

    CoArray      ca = coarray_new (n, y, z);
    CrossSection xs = xs_new (ca, n_roughness, r, z_r);

    n_roughness_test = xs_n_subsections (xs);

    r_test =
        Mem_calloc (n_roughness_test, sizeof (double), __FILE__, __LINE__);
    xs_roughness (xs, r_test);

    z_r_test =
        Mem_calloc (n_roughness_test - 1, sizeof (double), __FILE__, __LINE__);
    xs_z_roughness (xs, z_r_test);

    xs_free (xs);
    coarray_free (ca);
    Mem_free (r_test, __FILE__, __LINE__);
    Mem_free (z_r_test, __FILE__, __LINE__);
}

void
test_xs_critical_depth (void)
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

    CoArray           ca = coarray_new (n, y, z);
    CrossSection      xs = xs_new (ca, n_roughness, r, z_r);
    CrossSectionProps xsp;

    for (i = 1; i < 10; i++) {
        depth         = (double) i;
        xsp           = xs_hydraulic_properties (xs, depth);
        critical_flow = xsp_get (xsp, XS_CRITICAL_FLOW);
        xsp_free (xsp);

        critical_depth = xs_critical_depth (xs, critical_flow, 1.25 * depth);
        depth          = critical_depth; /* avoid warnings */
    }

    coarray_free (ca);
    xs_free (xs);
}

void
test_xs_normal_depth (void)
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

    CoArray           ca = coarray_new (n, y, z);
    CrossSection      xs = xs_new (ca, n_roughness, r, z_r);
    CrossSectionProps xsp;

    for (i = 1; i < 10; i++) {
        depth       = (double) i;
        xsp         = xs_hydraulic_properties (xs, depth);
        normal_flow = sqrt (slope) * xsp_get (xsp, XS_CONVEYANCE);
        xsp_free (xsp);

        normal_depth = xs_normal_depth (xs, normal_flow, slope, 1.25 * depth);
        depth        = normal_depth; /* avoid warnings */
    }

    coarray_free (ca);
    xs_free (xs);
}
