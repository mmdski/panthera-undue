#include <panthera/coarray.h>
#include <panthera/crosssection.h>
#include <panthera/reach.h>
#include <stdio.h>

int main(void) {

    int n                   = 9;
    double y[]              = {1,  0.5,   0,  0.5, 1,  0.5,   0,  0.5, 1};
    double z[]              = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2};
    double n_roughness      = 3;
    double roughness[]      = {0.05, 0.01, 0.05};
    double z_roughness[]    = {0.75, 1.25};

    CoArray ca = coarray_new(n, y, z);
    CrossSection xs;

    Reach reach = reach_new();

    int i;
    int n_x = 5;
    double x[] = {10, 50, 25, 5, 0};
    double *x_array = NULL;

    for (i = 0; i < n_x; i++) {
        printf("Adding cross section at x = %f\n", x[i]);
        xs = xs_new(ca, n_roughness, roughness, z_roughness);
        reach_put(reach, x[i], xs);
    }

    xs = xs_new(ca, n_roughness, roughness, z_roughness);
    reach_put(reach, x[0], xs);

    printf("The reach contains %i cross sections\n", reach_size(reach));

    n_x = reach_stream_distance(reach, &x_array);
    printf("n_x = %i\n", n_x);
    for (i = 0; i < n_x; i++)
        printf("x = %f\n", *(x_array + i));

    Mem_free(x_array, __FILE__, __LINE__);
    coarray_free(ca);
    reach_free(reach);

    return 0;
}
