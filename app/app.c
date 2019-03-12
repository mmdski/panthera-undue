#include "coarray.h"
#include "subsection.h"
#include <stdio.h>

int main() {

    int n                   = 5;
    double x[]              = {0, 0.25, 0.5, 0.75, 1};
    double y[]              = {0, 0.5, 1, 0.5, 0};
    double roughness        = 0.03;
    double activation_depth = 0;

    double test_depth = 0.5;

    Coordinate c;

    CoArray a    = coarray_new(n, x, y);
    CoArray sa = coarray_subarray_y(a, test_depth);
    int n_sa     = coarray_length(sa);

    Subsection ss    = subsection_new(n, x, y, roughness, activation_depth);
    double area      = subsection_area(ss, test_depth);
    double perimeter = subsection_perimeter(ss, test_depth);
    double width     = subsection_top_width(ss, test_depth);

    for (int i = 0; i < n_sa; i++) {
        c = coarray_get(sa, i);
        if (c == NULL)
            printf("NULL\n");
        else
            printf("x = %f\ty = %f\n", coord_x(c), coord_y(c));
    }

    printf("area = %f\n", area);
    printf("perimeter = %f\n", perimeter);
    printf("top width = %f\n", width);

    return 0;
}
