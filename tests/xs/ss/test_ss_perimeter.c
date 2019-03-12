#include "subsection.h"
#include <math.h>
#include <stdio.h>

int test_rectangle(double depth, double expected);
int test_right_triangle(double depth, double expected);

int main() {

    int result = 0;
    double expected;

    result = test_rectangle(1, 3);
    result = test_rectangle(0.5, 2);

    expected = sqrt(2);
    result   = test_right_triangle(1, expected);

    return result;
}

int test_rectangle(double depth, double expected) {
    printf("Starting test_rectangle\n");
    int result = 0;

    double calculated;

    int n                   = 4;
    double x[]              = {0, 0, 1, 1};
    double y[]              = {1, 0, 0, 1};
    double roughness        = 0.03;
    double activation_depth = 0.1;

    Subsection ss = ss_new(n, x, y, roughness, activation_depth);

    calculated = ss_perimeter(ss, depth);

    if (expected != calculated) {
        printf("test_rectangle failed\n");
        printf("expected = %f\tcalculated = %f\n", expected, calculated);
        result = 1;
    }

    ss_free(ss);

    return result;
}

int test_right_triangle(double depth, double expected) {
    printf("Starting test_right_triangle\n");
    int result = 0;

    double calculated;

    int n                   = 5;
    double x[]              = {0, 0.25, 0.5, 0.75, 1};
    double y[]              = {0, 0.25, 0.5, 0.75, 1};
    double roughness        = 0.03;
    double activation_depth = 0.1;

    Subsection ss = ss_new(n, x, y, roughness, activation_depth);

    calculated = ss_perimeter(ss, depth);

    if (expected != calculated) {
        printf("test_right_triangle failed\n");
        printf("expected = %f\tcalculated = %f\n", expected, calculated);
        result = 1;
    }

    ss_free(ss);

    return result;
}
