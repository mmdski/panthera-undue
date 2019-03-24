#include "subsection.h"
#include <stdio.h>
#include "test_xs.h"

#define ABS_TOL 1e-15
#define REL_TOL 0

typedef struct {
    Subsection ss;
} ss_fixture;

void check_area(ss_fixture *ssf, xs_test_data test_data, double depth) {
    double calculated_area = ss_area(ssf->ss, depth);
    double expected_area   = calc_area(test_data, depth);
    bool is_close = test_is_close(calculated_area, expected_area,
                                           ABS_TOL, REL_TOL);
    g_assert_true(is_close);
}

void check_perimeter(ss_fixture *ssf, xs_test_data test_data, double depth) {
    double calculated = ss_perimeter(ssf->ss, depth);
    double expected   = calc_perimeter(test_data, depth);
    bool is_close = test_is_close(calculated, expected, ABS_TOL, REL_TOL);
    if (!is_close)
        printf("calculated = %f\texpected = %f\n", calculated, expected);
    g_assert_true(is_close);
}

void check_top_width(ss_fixture *ssf, xs_test_data test_data, double depth) {
    double calculated = ss_top_width(ssf->ss, depth);
    double expected   = calc_top_width(test_data, depth);
    g_assert_true(test_is_close(calculated, expected, ABS_TOL, REL_TOL));
}

void init_ss(ss_fixture *ssf, xs_test_data test_data) {
    CoArray ca = coarray_new(test_data.n_coords, test_data.y, test_data.z);
    ssf->ss = ss_new(ca, *(test_data.roughness), test_data.activation_depth);
    coarray_free(ca);
}

void ss_setup(ss_fixture *ssf, gconstpointer test_data) {
    init_ss(ssf, *(const xs_test_data *)test_data);
}

void ss_teardown(ss_fixture *ssf, gconstpointer test_data) {
    ss_free(ssf->ss);
    xs_test_data_free((xs_test_data *)test_data);
}

void test_h_properties(ss_fixture *ssf, gconstpointer test_data) {

    int steps = 10;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double)i / (double)steps;
        check_area(ssf, *(const xs_test_data *)test_data, depth);
        check_perimeter(ssf, *(const xs_test_data *)test_data, depth);
        check_top_width(ssf, *(const xs_test_data *)test_data, depth);
    }
}

void add_rect_test() {

    int n                        = 5;
    double y[]                   = {0, 0, 0.5, 1, 1};
    double z[]                   = {1, 0, 0, 0, 1};
    double r[]                   = {0.030}; /* roughness */
    xs_test_data *rect_test_data = xs_test_data_new(n, y, z, 1, r, 'r');
    g_test_add("/panthera/xs/subsection/results test - simple rectangle",
               ss_fixture, rect_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_triangle_test() {
    int n                            = 5;
    double y[]                       = {0, 0.25, 0.5, 0.75, 1};
    double z[]                       = {1, 0.5, 0, 0.5, 1};
    double r[]                       = {0.030};
    xs_test_data *triangle_test_data = xs_test_data_new(n, y, z, 1, r, 't');
    g_test_add("/panthera/xs/subsection/results test - simple triangle",
               ss_fixture, triangle_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_trapezoid_test() {
    int n                             = 6;
    double y[]                        = {0, 0.25, 0.5, 1.5, 1.75, 2};
    double z[]                        = {1, 0.5, 0, 0, 0.5, 1};
    double r[]                        = {0.030};
    xs_test_data *trapezoid_test_data = xs_test_data_new(n, y, z, 1, r, 'z');
    g_test_add("/panthera/xs/subsection/results test - simple trapezoid",
               ss_fixture, trapezoid_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_subsection_tests() {
    add_rect_test();
    add_triangle_test();
    add_trapezoid_test();
}
