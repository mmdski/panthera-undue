#include "coarray.h"
#include "subsection.h"
#include <stdio.h>

int main() {

    int n                   = 6;
    double x[]              = {0, 0.1, 0.2, 0.3, 0.4, 0.5};
    double y[]              = {0, 0.25, 0.5, 0.5, 0, 0.5};
    double roughness        = 0.03;
    double activation_depth = 0;

    double test_depth = 0.3;

    Coordinate_T c;

    CoArray_T a  = coarray_new(n, x, y);
    CoArray_T sa = coarray_subarray_y(a, test_depth);
    int n_sa     = coarray_n(sa);

    Subsection_T ss = subsection_new(n, x, y, roughness, activation_depth);
    double area     = subsection_area(ss, test_depth);

    for (int i = 0; i < n_sa; i++) {
        c = coarray_get(sa, i);
        if (c == NULL)
            printf("NULL\n");
        else
            printf("x = %f\ty = %f\n", coord_x(c), coord_y(c));
    }

    printf("area = %f\n", area);

    return 0;
}
