#include "coarray.h"
#include <stddef.h>

int test_empty(CoArray_T a);
int test_half(CoArray_T a);
int test_all(CoArray_T a);

int main() {

    int result = 0;

    int n      = 2;
    double x[] = {0, 1};
    double y[] = {1, 2};

    CoArray_T a = coarray_new(n, x, y);

    if (test_empty(a))
        result = 1;

    if (test_half(a))
        result = 1;

    if (test_all(a))
        result = 1;

    coarray_free(a);

    return result;
}

int test_empty(CoArray_T a) {

    int result = 0;

    double d = 0.5;

    CoArray_T sa = coarray_subarray_y(a, d);

    if (coarray_n(sa) != 0)
        result = 1;

    coarray_free(sa);

    return result;
}

int test_half(CoArray_T a) {

    int result = 0;

    double d = 1.5;

    int n      = 2;
    double x[] = {0, 0.5};
    double y[] = {1, 1.5};

    CoArray_T expected = coarray_new(n, x, y);

    CoArray_T sa = coarray_subarray_y(a, d);

    if (!coarray_eq(sa, expected))
        result = 1;

    coarray_free(expected);
    coarray_free(sa);

    return result;
}

int test_all(CoArray_T a) {

    int result = 0;

    double d = 10;

    int n      = 2;
    double x[] = {0, 1};
    double y[] = {1, 2};

    CoArray_T expected = coarray_new(n, x, y);

    CoArray_T sa = coarray_subarray_y(a, d);

    if (!coarray_eq(sa, expected))
        result = 1;

    coarray_free(expected);
    coarray_free(sa);

    return result;
}
