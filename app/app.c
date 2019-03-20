#include "panthera/coarray.h"
#include "panthera/crosssection.h"
#include <stdio.h>

int main() {

    int n                   = 5;
    double y[]              = {0, 0.25, 0.5, 0.75, 1};
    double z[]              = {0, 0.5, 1, 0.5, 0};
    double n_roughness      = 2;
    double roughness[]      = {0.03, 0.03};
    double y_roughness[]    = {1};

    CoArray ca = coarray_new(n, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, y_roughness);
    xs_free(xs);
    coarray_free(ca);

    return 0;
}
