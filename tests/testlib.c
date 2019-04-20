#include "testlib.h"

/* no exception placeholder */
const Except_T no_error;

CrossSection
new_cross_section(void)
{

    int    n_coords = 5;
    double y[]      = {1, 0, 0, 0, 1};
    double z[]      = {0, 0, 0.5, 1, 1};

    double  n_roughness = 1;
    double  roughness[] = {0.03};
    double *z_roughness = NULL;

    CoArray      ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    return xs;
}

bool
test_is_close(double a, double b, double abs_tol, double rel_tol)
{
    if (fabs(a - b) <= (abs_tol + rel_tol * fabs(b))) {
        return true;
    } else {
        return false;
    }
}
