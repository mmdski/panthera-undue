#include "cii/except.h"
#include "coarray.h"
#include <stdio.h>

int test_new(int n, double *x, double *y);
int test_new_succeed();
int test_new_fail();

int main() {
    if (test_new_succeed() != 0)
        return 1;
    if (test_new_fail() != 0)
        return 1;
    return 0;
}

int test_new_succeed() {
    int result;

    /* first test, expecting to succeed */
    int n      = 10;
    double x[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    double y[] = {1, 0.75, 0.5, 0.25, 0, 0, 0.25, 0.5, 0.75, 1};

    result = test_new(n, x, y);

    if (result != 0)
        return 1;
    else
        return 0;
}

int test_new_fail() {
    int result;

    /* second test, expecting to fail */
    int n      = 10;
    double x[] = {0, 0.1, 0.3, 0.2, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    double y[] = {1, 0.75, 0.5, 0.25, 0, 0, 0.25, 0.5, 0.75, 1};

    result = test_new(n, x, y);

    if (result != 0)
        return 0;
    else
        return 1;
}

/* 0 - success
   1 - failure */
int test_new(int n, double *x, double *y) {
    int result = 0;
    CoArray_T a;

    TRY a = coarray_new(n, x, y);
    EXCEPT(coarray_x_order_Error) { result = 1; }
    END_TRY;

    if (result == 0)
        coarray_free(a);

    return result;
}
