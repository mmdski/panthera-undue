#include "cii/mem.h"
#include "subsection.h"
#include "testlib.h"
#include <stdio.h>

#define ABS_TOL 1e-10
#define REL_TOL 1

typedef struct {
    Subsection ss;
} ss_fixture;

struct ss_test_data {

    /* initialization data */
    int n_coords;
    double *x;
    double *z;
    double roughness;
    double activation_depth;

    /* shape information */
    /* r - rectangle */
    /* t - triangle */
    /* z - trapezoid */
    char shape;

    /* dimensions for analytical solutions */
    double b0; /* bottom width */
    double s;  /* slope */
};

typedef struct ss_test_data ss_test_data;

double calc_area(ss_test_data test_data, double depth) {
    double area;

    if (test_data.shape == 'r')
        area = test_data.b0 * depth;
    else if (test_data.shape == 't')
        area = test_data.s * depth * depth;
    else if (test_data.shape == 'z')
        area = (test_data.b0 + test_data.s * depth) * depth;
    else
        g_assert_not_reached();
    return area;
}

double calc_perimeter(ss_test_data test_data, double depth) {
    double perimeter;

    if (test_data.shape == 'r')
        perimeter = test_data.b0 + 2 * depth;
    else if (test_data.shape == 't')
        perimeter = 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else if (test_data.shape == 'z')
        perimeter =
            test_data.b0 + 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else
        g_assert_not_reached();

    return perimeter;
}

double calc_top_width(ss_test_data test_data, double depth) {
    double top_width;

    if (test_data.shape == 'r')
        top_width = test_data.b0;
    else if (test_data.shape == 't')
        top_width = 2 * test_data.s * depth;
    else if (test_data.shape == 'z')
        top_width = test_data.b0 + 2 * test_data.s * depth;
    else
        g_assert_not_reached();

    return top_width;
}

void check_area(ss_fixture *ssf, ss_test_data test_data, double depth) {
    double calculated_area = ss_area(ssf->ss, depth);
    double expected_area   = calc_area(test_data, depth);
    g_assert_true(
        test_is_close(calculated_area, expected_area, ABS_TOL, REL_TOL));
}

void check_perimeter(ss_fixture *ssf, ss_test_data test_data, double depth) {
    double calculated = ss_perimeter(ssf->ss, depth);
    double expected   = calc_perimeter(test_data, depth);
    g_assert_true(test_is_close(calculated, expected, ABS_TOL, REL_TOL));
}

void check_top_width(ss_fixture *ssf, ss_test_data test_data, double depth) {
    double calculated = ss_top_width(ssf->ss, depth);
    double expected   = calc_top_width(test_data, depth);
    g_assert_true(test_is_close(calculated, expected, ABS_TOL, REL_TOL));
}

ss_test_data *ss_test_data_new(int n, double *x, double *z, char shape) {

    ss_test_data *test_data =
        (ss_test_data *)Mem_alloc(sizeof(ss_test_data), __FILE__, __LINE__);
    test_data->n_coords  = n;
    test_data->x         = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    test_data->z         = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    test_data->shape     = shape;
    test_data->roughness = 0.03;
    test_data->b0        = 1;
    test_data->s         = 0.5;

    for (int i = 0; i < test_data->n_coords; i++) {
        *(test_data->x + i) = x[i];
        *(test_data->z + i) = z[i];
    }

    return test_data;
}

void ss_test_data_free(ss_test_data *test_data) {
    Mem_free(test_data->x, __FILE__, __LINE__);
    Mem_free(test_data->z, __FILE__, __LINE__);
    Mem_free(test_data, __FILE__, __LINE__);
}

void init_ss(ss_fixture *ssf, ss_test_data test_data) {
    ssf->ss = ss_new(test_data.n_coords, test_data.x, test_data.z,
                     test_data.roughness, test_data.activation_depth);
}

void ss_setup(ss_fixture *ssf, gconstpointer test_data) {
    init_ss(ssf, *(const ss_test_data *)test_data);
}

void ss_teardown(ss_fixture *ssf, gconstpointer test_data) {
    ss_free(ssf->ss);
    ss_test_data_free((ss_test_data *)test_data);
}

void test_h_properties(ss_fixture *ssf, gconstpointer test_data) {

    int steps = 10;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double)i / (double)steps;
        check_area(ssf, *(const ss_test_data *)test_data, depth);
        check_perimeter(ssf, *(const ss_test_data *)test_data, depth);
        check_top_width(ssf, *(const ss_test_data *)test_data, depth);
    }
}

void add_rect_test() {

    int n                        = 5;
    double x[]                   = {0, 0, 0.5, 1, 1};
    double z[]                   = {1, 0, 0, 0, 1};
    ss_test_data *rect_test_data = ss_test_data_new(n, x, z, 'r');
    g_test_add("/panthera/xs/subsection/results test - simple rectangle",
               ss_fixture, rect_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_triangle_test() {
    int n                            = 5;
    double x[]                       = {0, 0.25, 0.5, 0.75, 1};
    double z[]                       = {1, 0.5, 0, 0.5, 1};
    ss_test_data *triangle_test_data = ss_test_data_new(n, x, z, 't');
    g_test_add("/panthera/xs/subsection/results test - simple triangle",
               ss_fixture, triangle_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_trapezoid_test() {
    int n                             = 6;
    double x[]                        = {0, 0.25, 0.5, 1.5, 1.75, 2};
    double z[]                        = {1, 0.5, 0, 0, 0.5, 1};
    ss_test_data *trapezoid_test_data = ss_test_data_new(n, x, z, 'z');
    g_test_add("/panthera/xs/subsection/results test - simple trapezoid",
               ss_fixture, trapezoid_test_data, ss_setup, test_h_properties,
               ss_teardown);
}

void add_subsection_tests() {
    add_rect_test();
    add_triangle_test();
    add_trapezoid_test();
}
