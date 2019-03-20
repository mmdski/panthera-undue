#include "panthera/coarray.h"
#include "panthera/subsection.h"
#include <stdio.h>

int main() {

    int n                   = 5;
    double y[]              = {0, 0.25, 0.5, 0.75, 1};
    double z[]              = {0, 0.5, 1, 0.5, 0};
    double roughness        = 0.03;
    double activation_depth = 0;

    double test_depth = 0.5;

    Coordinate c;

    CoArray a    = coarray_new(n, y, z);
    CoArray sa = coarray_subarray_z(a, test_depth);
    int n_sa     = coarray_length(sa);

    Subsection ss    = ss_new(a, roughness, activation_depth);
    double area      = ss_area(ss, test_depth);
    double perimeter = ss_perimeter(ss, test_depth);
    double width     = ss_top_width(ss, test_depth);

    for (int i = 0; i < n_sa; i++) {
        c = coarray_get(sa, i);
        if (c == NULL)
            printf("NULL\n");
        else
            printf("x = %f\ty = %f\n", coord_y(c), coord_z(c));
        coord_free(c);
    }

    ss_free(ss);
    coarray_free(a);
    coarray_free(sa);

    printf("area = %f\n", area);
    printf("perimeter = %f\n", perimeter);
    printf("top width = %f\n", width);

    return 0;
}
