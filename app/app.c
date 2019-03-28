#include "panthera/crosssection.h"
#include <stdio.h>

int main() {

    int n                   = 9;
    double y[]              = {1,  0.5,   0,  0.5, 1,  0.5,   0,  0.5, 1};
    double z[]              = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2};
    double n_roughness      = 3;
    double roughness[]      = {0.05, 0.01, 0.05};
    double z_roughness[]    = {0.75, 1.25};

    CoArray ca = coarray_new(n, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);

    CoArray xs_ca = xs_coarray(xs);
    HydraulicProps hp;

    double depth;
    double max_depth = 1;
    double increments = 5;

    for (depth = 0; depth <= max_depth; depth += max_depth/increments) {

        hp = xs_hydraulic_properties(xs, depth);
        printf("\n");
        printf("depth            = %f\n", hp_get(hp, HP_DEPTH));
        printf("area             = %f\n", hp_get(hp, HP_AREA));
        printf("top_width        = %f\n", hp_get(hp, HP_TOP_WIDTH));
        printf("wetted perimeter = %f\n", hp_get(hp, HP_WETTED_PERIMETER));
        printf("hydraulic depth  = %f\n", hp_get(hp, HP_HYDRAULIC_DEPTH));
        printf("conveyance       = %f\n", hp_get(hp, HP_CONVEYANCE));
        hp_free(hp);
    }

    printf("\n");
    if (coarray_eq(ca, xs_ca)) {
        printf("CoArrays are equal\n");
    } else {
        printf("CoArrays are not equal\n");
    }

    xs_free(xs);
    coarray_free(ca);
    coarray_free(xs_ca);

    return 0;
}
