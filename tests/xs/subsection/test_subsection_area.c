#include "subsection.h"
#include <math.h>
#include <stdio.h>

int test_rectangle();
int test_simple_triangle();
int test_equilateral_triangle();

int main() {

    int result = 0;

    if (test_rectangle()) {
        result = 1;
        printf("test_rectangle failed\n");
    }

    if (test_simple_triangle()) {
        result = 1;
        printf("test_simple_triangle failed\n");
    }

    if (test_equilateral_triangle()) {
        result = 1;
        printf("test_equilateral_triangle failed\n");
    }

    return result;
}

int test_rectangle() {

    double expected_area;
    double calculated_area;

    int result = 0;

    int n                   = 7;
    double x[]              = {0, 0, 0.25, 0.5, 0.75, 1, 1};
    double y[]              = {1, 0, 0, 0, 0, 0, 1};
    double roughness        = 0.03;
    double activation_depth = 0.1;

    Subsection ss = subsection_new(n, x, y, roughness, activation_depth);

    /* below activation depth
     * expected area is 0
     */
    expected_area   = 0;
    calculated_area = subsection_area(ss, activation_depth / 2);
    if (expected_area != calculated_area)
        result = 1;

    /* depth of 0.5
     * 0.5 * 1 = 0.5
     * expected area is 0.5
     */
    expected_area   = 0.5;
    calculated_area = subsection_area(ss, 0.5);
    if (fabs(expected_area - calculated_area) > 1e-15)
        result = 1;

    /* depth of 1
     * 1 * 1 = 1
     * expected area is 1
     */
    expected_area   = 1;
    calculated_area = subsection_area(ss, 1);
    if (fabs(expected_area - calculated_area) > 1e-15)
        result = 1;

    subsection_free(ss);

    return result;
}

int test_simple_triangle() {

    double expected_area;
    double calculated_area;

    int result = 0;

    int n                   = 5;
    double x[]              = {0, 0.25, 0.5, 0.75, 1};
    double y[]              = {0, 0.25, 0.5, 0.75, 1};
    double roughness        = 0.03;
    double activation_depth = 0;

    Subsection ss = subsection_new(n, x, y, roughness, activation_depth);

    /* depth of 1
     * 0.5 * 1 * 1 = 0.5
     * expected area is 0.5
     */
    expected_area   = 0.5;
    calculated_area = subsection_area(ss, 1);
    if (fabs(expected_area - calculated_area) > 1e-15)
        result = 1;

    subsection_free(ss);

    return result;
}

int test_equilateral_triangle() {

    double expected_area;
    double calculated_area;

    int result = 0;

    int n                   = 5;
    double x[]              = {0, 0.25, 0.5, 0.75, 1};
    double y[]              = {0, 0.5, 1, 0.5, 0};
    double roughness        = 0.03;
    double activation_depth = 0;

    Subsection ss = subsection_new(n, x, y, roughness, activation_depth);

    /* depth of 1
     * 0.5 * 1 * 1 = 0.5
     * expected area is 0.5
     */
    expected_area   = 0.5;
    calculated_area = subsection_area(ss, 1);
    if (fabs(expected_area - calculated_area) > 1e-15)
        result = 1;

    /* depth of 0.5
     * 2*0.5*0.5*0.25
     * expected area is 0.125
     */
    expected_area   = 0.125;
    calculated_area = subsection_area(ss, 0.5);
    if (fabs(expected_area - calculated_area) > 1e-15) {
        result = 1;
        printf("expected_area = %f\tcalculated_area = %f\n", expected_area,
               calculated_area);
    }

    subsection_free(ss);

    return result;
}
