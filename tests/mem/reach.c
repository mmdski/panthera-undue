#include <panthera/reach.h>
#include <stddef.h>

CrossSection new_cross_section(void) {

    int n_coords = 5;
    double y[]   = {1, 0, 0, 0, 1};
    double z[]   = {0, 0, 0.5, 1, 1};

    double n_roughness      = 1;
    double roughness[]      = {0.03};
    double *z_roughness     = NULL;

    CoArray ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    return xs;
}

int main(void) {
    int n_nodes = 5;

    double x[]      = {1, 2, 3, 4, 5};
    double y[]      = {0.01, 0.02, 0.03, 0.04, 0.05};
    int xs_number[] = {0, 0, 0, 0, 0};

    CrossSection xs;
    XSTable xstable;

    Reach reach;

    xs = new_cross_section();
    xstable = xstable_new();

    xstable_put(xstable, 0, xs);

    reach = reach_new(n_nodes, x, y, xs_number, xstable);

    reach_free(reach);
    xstable_free(xstable);
}
