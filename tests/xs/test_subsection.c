#include "cii/mem.h"
#include "panthera_test.h"
#include "subsection.h"
#include <stdbool.h>
#include <stdio.h>

#define REL_TOL 1e-10

typedef struct {
    Subsection ss;
} ss_fixture;

struct ss_test_data {

    /* initialization data */
    int n_coords;
    double *x;
    double *y;
    double roughness;
    double activation_depth;

    /* depth for calculating results */
    double depth;

    /* shape information */
    /* r - rectangle */
    char shape;

    /* dimensions for analytical solutions */
    double dim1;
    double dim2;
    double dim3;
};

typedef struct ss_test_data ss_test_data;

void print_ss_test_data(ss_test_data test_data) {
    int i;

    printf("\nss_test_data->n_coords = \t%i\n", test_data.n_coords);
    printf("x\t\ty\n");
    for (i = 0; i < test_data.n_coords; i++) {
        printf("%f\t%f\n", test_data.x[i], test_data.y[i]);
    }
}

double calc_area(ss_test_data test_data) {
    double area;

    if (test_data.shape == 'r') {
        /* dim1 is width */
        area = test_data.dim1 * test_data.depth;

    } else {
        g_assert_not_reached();
    }

    return area;
}

void check_area(ss_fixture *ssf, ss_test_data test_data) {
    double calculated_area = ss_area(ssf->ss, test_data.depth);
    double expected_area   = calc_area(test_data);
    bool area_is_close =
        (bool)test_is_close(calculated_area, expected_area, REL_TOL);
    if (!area_is_close)
        printf("area_is_close is false\n");
    g_assert_true(area_is_close);
}

void check_results(ss_fixture *ssf, ss_test_data test_data) {
    check_area(ssf, test_data);
}

void init_ss(ss_fixture *ssf, ss_test_data test_data) {
    ssf->ss = ss_new(test_data.n_coords, test_data.x, test_data.y,
                     test_data.roughness, test_data.activation_depth);
}

void ss_setup(ss_fixture *ssf, gconstpointer test_data) {
    init_ss(ssf, *(const ss_test_data *)test_data);
}

void ss_teardown(ss_fixture *ssf, gconstpointer test_data) {
    ss_free(ssf->ss);
}

void test_results(ss_fixture *ssf, gconstpointer test_data) {
    check_results(ssf, *(const ss_test_data *)test_data);
}

void add_rect_tests() {

    ss_test_data *rect_test_data =
        (ss_test_data *)Mem_alloc(sizeof(ss_test_data), __FILE__, __LINE__);
    rect_test_data->n_coords = 5;
    rect_test_data->x = Mem_calloc(rect_test_data->n_coords, sizeof(double),
                                   __FILE__, __LINE__);
    rect_test_data->y = Mem_calloc(rect_test_data->n_coords, sizeof(double),
                                   __FILE__, __LINE__);
    rect_test_data->shape     = 'r';
    rect_test_data->roughness = 0.03;
    rect_test_data->depth     = 0.5;
    rect_test_data->dim1      = 1;

    double x[] = {0, 0, 0.5, 1, 1};
    double y[] = {1, 0, 0, 0, 1};

    for (int i = 0; i < rect_test_data->n_coords; i++) {
        *(rect_test_data->x + i) = x[i];
        *(rect_test_data->y + i) = y[i];
    }
    g_test_add("/xs/subsection/results test - simple rectangle", ss_fixture,
               rect_test_data, ss_setup, test_results, ss_teardown);
}

void add_subsection_tests() { add_rect_tests(); }
