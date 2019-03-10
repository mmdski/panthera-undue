#include "coarray_test.h"

void print_coarray_fail(CoArray_T expected, CoArray_T result) {
    printf("expected\n");
    print_coarray(expected);
    printf("\nresult\n");
    print_coarray(result);
}

void print_coarray(CoArray_T a) {

    int i;
    int n = coarray_n(a);

    Coordinate_T c;

    printf("n = %i\n", n);

    for (i = 0; i < n; i++) {
        c = coarray_get(a, i);
        printf("x = %f\ty = %f\n", coord_x(c), coord_y(c));
    }
}
