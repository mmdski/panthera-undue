#include "coarray.h"
#include <stddef.h>
#include <stdio.h>

void print_coarray(CoArray a);

int test_empty(CoArray a);
int test_half(CoArray a);
int test_all(CoArray a);
// int test_equilateral_triangle();

int main() {

    int result = 0;

    int n      = 2;
    double x[] = {0, 1};
    double y[] = {1, 2};

    CoArray a = coarray_new(n, x, y);

    if (test_empty(a)) {
        result = 1;
        printf("test_empty failed\n");
    }

    if (test_half(a)) {
        result = 1;
        printf("test_half failed\n");
    }

    if (test_all(a)) {
        result = 1;
        printf("test_all failed\n");
    }

    // if (test_equilateral_triangle()) {
    // result = 1;
    // printf("test_equilateral_triangle failed\n");
    // }

    coarray_free(a);

    return result;
}

void print_coarray(CoArray a) {
    int n = coarray_length(a);
    Coordinate c;
    for (int i = 0; i < n; i++) {
        c = coarray_get(a, i);
        if (c == NULL)
            printf("NULL\n");
        else
            printf("x = %f\ty = %f\n", coord_x(c), coord_y(c));
    }
}

int test_empty(CoArray a) {

    int result = 0;

    double d = 0.5;

    CoArray sa = coarray_subarray_y(a, d);

    if (coarray_length(sa) != 0) {
        result = 1;
        print_coarray(sa);
    }

    coarray_free(sa);

    return result;
}

int test_half(CoArray a) {

    int result = 0;

    double d = 1.5;

    int n      = 2;
    double x[] = {0, 0.5};
    double y[] = {1, 1.5};

    CoArray expected = coarray_new(n, x, y);

    CoArray sa = coarray_subarray_y(a, d);

    if (!coarray_eq(sa, expected)) {
        result = 1;
        print_coarray(sa);
    }

    coarray_free(expected);
    coarray_free(sa);

    return result;
}

int test_all(CoArray a) {

    int result = 0;

    double d = 10;

    int n      = 2;
    double x[] = {0, 1};
    double y[] = {1, 2};

    CoArray expected = coarray_new(n, x, y);

    CoArray sa = coarray_subarray_y(a, d);

    if (!coarray_eq(sa, expected)) {
        result = 1;
        print_coarray(sa);
    }

    coarray_free(expected);
    coarray_free(sa);

    return result;
}

int test_equilateral_triangle() {

    int result = 0;

    int n      = 5;
    double x[] = {0, 0.25, 0.5, 0.75, 1};
    double y[] = {0, 0.5, 1, 0.5, 0};

    CoArray a  = coarray_new(n, x, y);
    CoArray sa = coarray_subarray_y(a, 0.5);

    printf("equilateral triangle not really tested\n");
    print_coarray(sa);

    return result;
}
