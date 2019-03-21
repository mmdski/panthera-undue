#include "panthera.h"
#include <stdio.h>

int main() {

    int n                   = 9;
    double y[]              = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2};
    double z[]              = {1,  0.5,   0,  0.5, 1,  0.5,   0,  0.5, 1};
    double n_roughness      = 2;
    double roughness[]      = {0.03, 0.03};
    double y_roughness[]    = {1};

    CoArray ca = coarray_new(n, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, y_roughness);

    CoArray xs_ca = xs_coarray(xs);

    HydraulicProps hp = xs_hydraulic_properties(xs, 1);

    printf("area             = %f\n", hp_get_property(hp, HP_AREA));
    printf("top_width        = %f\n", hp_get_property(hp, HP_TOP_WIDTH));
    printf("wetted perimeter = %f\n", hp_get_property(hp,
                                                      HP_WETTED_PERIMETER));
    printf("hydraulic depth  = %f\n", hp_get_property(hp, HP_HYDRAULIC_DEPTH));

    if (coarray_eq(ca, xs_ca)) {
        printf("CoArrays are equal\n");
    } else {
        printf("CoArrays are not equal\n");
    }

    xs_free(xs);
    coarray_free(ca);
    coarray_free(xs_ca);
    hp_free(hp);

    return 0;
}
