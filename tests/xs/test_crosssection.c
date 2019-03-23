#include <panthera/crosssection.h>
#include "test_xs.h"

#define ABS_TOL 1e-15
#define REL_TOL 0

typedef struct {
    CrossSection xs;
} xs_fixture;

void check_simple_hp(xs_fixture *xsf, xs_test_data test_data, double depth) {
    HydraulicProps hp = xs_hydraulic_properties(xsf->xs, depth);

    double calculated;
    double expected;
    bool is_close;

    calculated = hp_get(hp, HP_AREA);
    expected   = calc_area(test_data, depth);
    is_close   = test_is_close(calculated, expected, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    calculated = hp_get(hp, HP_TOP_WIDTH);
    expected   = calc_top_width(test_data, depth);
    is_close   = test_is_close(calculated, expected, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    calculated = hp_get(hp, HP_WETTED_PERIMETER);
    expected   = calc_perimeter(test_data, depth);
    is_close   = test_is_close(calculated, expected, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    calculated = hp_get(hp, HP_HYDRAULIC_DEPTH);
    expected   = calc_hydraulic_depth(test_data, depth);
    is_close   = test_is_close(calculated, expected, ABS_TOL, REL_TOL);
    g_assert_true(is_close);
}

void init_xs(xs_fixture *xsf, xs_test_data test_data) {
    CoArray ca = coarray_new(test_data.n_coords, test_data.y, test_data.z);
    xsf->xs = xs_new(ca, test_data.n_roughness, test_data.roughness,
                     test_data.y_roughness);
    coarray_free(ca);
}

void xs_setup(xs_fixture *xsf, gconstpointer test_data) {
    init_xs(xsf, *(const xs_test_data *)test_data);
}

void xs_teardown(xs_fixture *xsf, gconstpointer test_data) {
    xs_free(xsf->xs);
    xs_test_data_free((xs_test_data *)test_data);
}

void test_simple_h_properties(xs_fixture *xsf, gconstpointer test_data) {

    int steps = 10;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double)i / (double)steps;
        check_simple_hp(xsf, *(const xs_test_data *)test_data, depth);
    }
}

void add_xs_rect_test() {

    int n                        = 5;
    double y[]                   = {0, 0, 0.5, 1, 1};
    double z[]                   = {1, 0, 0, 0, 1};
    xs_test_data *rect_test_data = xs_test_data_new(n, y, z, 'r');
    g_test_add("/panthera/xs/crosssection/results test - simple rectangle",
               xs_fixture, rect_test_data, xs_setup, test_simple_h_properties,
               xs_teardown);
}

void add_crosssection_tests() {
    add_xs_rect_test();
}
