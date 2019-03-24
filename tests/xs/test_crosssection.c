#include <panthera/crosssection.h>
#include "test_xs.h"

#define ABS_TOL 1e-15
#define REL_TOL 0

typedef struct {
    CrossSection xs;
    const Except_T *caught_exception;
} xs_fixture;

void init_xs(xs_fixture *xsf, xs_test_data test_data) {
    TRY
        CoArray ca = coarray_new(test_data.n_coords, test_data.y, test_data.z);
        xsf->xs = xs_new(ca, test_data.n_roughness, test_data.roughness,
                         test_data.y_roughness);
        coarray_free(ca);
        xsf->caught_exception = &no_Error;
    EXCEPT(null_ptr_arg_Error)
        xsf->caught_exception = &null_ptr_arg_Error;
    EXCEPT(value_arg_Error)
        xsf->caught_exception = &value_arg_Error;
    END_TRY;
}

void xs_setup(xs_fixture *xsf, gconstpointer test_data) {
    init_xs(xsf, *(const xs_test_data *)test_data);
}

void xs_teardown(xs_fixture *xsf, gconstpointer test_data) {
    if (xsf->xs)
        xs_free(xsf->xs);
    xs_test_data_free((xs_test_data *)test_data);
}

void test_xs_new(xs_fixture *xsf, gconstpointer test_data) {
    xs_test_data data = *(const xs_test_data *)test_data;
    g_assert_true(xsf->caught_exception == data.exception);
}

void check_simple_hp(xs_fixture *xsf, xs_test_data test_data, double depth) {
    HydraulicProps hp = xs_hydraulic_properties(xsf->xs, depth);

    bool is_close;

    double calculated_area = hp_get(hp, HP_AREA);
    double expected_area   = calc_area(test_data, depth);
    is_close   = test_is_close(calculated_area, expected_area, ABS_TOL,
                               REL_TOL);
    g_assert_true(is_close);

    double calculated_tw = hp_get(hp, HP_TOP_WIDTH);
    double expected_tw   = calc_top_width(test_data, depth);
    is_close   = test_is_close(calculated_tw, expected_tw, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_wp = hp_get(hp, HP_WETTED_PERIMETER);
    double expected_wp   = calc_perimeter(test_data, depth);
    is_close   = test_is_close(calculated_wp, expected_wp, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_hd = hp_get(hp, HP_HYDRAULIC_DEPTH);
    double expected_hd   = calc_hydraulic_depth(test_data, depth);
    is_close   = test_is_close(calculated_hd, expected_hd, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_hr = hp_get(hp, HP_HYDRAULIC_RADIUS);
    double expected_hr   = calc_hydraulic_radius(test_data, depth);
    is_close   = test_is_close(calculated_hr, expected_hr, ABS_TOL, REL_TOL);
    g_assert_true(is_close);
}

void test_simple_h_properties(xs_fixture *xsf, gconstpointer test_data) {

    int steps = 10;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double)i / (double)steps;
        check_simple_hp(xsf, *(const xs_test_data *)test_data, depth);
    }
}

void test_xs_hp_fail(void) {
    TRY
        xs_hydraulic_properties(NULL, 1);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error)
        ;
    END_TRY;
}

void test_xs_coarray_fail(void) {
    TRY
        xs_coarray(NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error)
        ;
    END_TRY;
}

void add_xs_new_test() {
    int n                        = 5;
    double y[]                   = {0, 0, 0.5, 1, 1};
    double z[]                   = {1, 0, 0, 0, 1};
    double r[]                   = {0.030};
    double zero_r[]              = {0};
    double neg_r[]               = {-0.030};

    xs_test_data *rect_test_data = xs_test_data_new(n, y, z, 1, r, 'r');
    rect_test_data->exception = &no_Error;
    g_test_add("/panthera/xs/crosssection/new/no error",
               xs_fixture, rect_test_data, xs_setup, test_xs_new, xs_teardown);

    xs_test_data *zero_n_roughness_test_data =
        xs_test_data_new(n, y, z, 1, r, 'r');
    zero_n_roughness_test_data->n_roughness = 0;
    zero_n_roughness_test_data->exception = &value_arg_Error;
    g_test_add("/panthera/xs/crosssection/new/zero n_roughness",
               xs_fixture, zero_n_roughness_test_data, xs_setup, test_xs_new,
               xs_teardown);

    xs_test_data *zero_roughness_test_data =
        xs_test_data_new(n, y, z, 1, zero_r, 'r');
    zero_roughness_test_data->exception = &value_arg_Error;
    g_test_add("/panthera/xs/crosssection/new/zero roughness",
               xs_fixture, zero_roughness_test_data, xs_setup, test_xs_new,
               xs_teardown);

    xs_test_data *neg_roughness_test_data =
        xs_test_data_new(n, y, z, 1, neg_r, 'r');
    neg_roughness_test_data->exception = &value_arg_Error;
    g_test_add("/panthera/xs/crosssection/new/negative roughness",
               xs_fixture, neg_roughness_test_data, xs_setup, test_xs_new,
               xs_teardown);

    xs_test_data *null_roughness_test_data =
        xs_test_data_new(n, y, z, 1, r, 'r');
    null_roughness_test_data->roughness = NULL;
    null_roughness_test_data->exception = &null_ptr_arg_Error;
    g_test_add("/panthera/xs/crosssection/new/null roughness",
               xs_fixture, null_roughness_test_data, xs_setup, test_xs_new,
               xs_teardown);

    xs_test_data *null_y_roughness_test_data =
        xs_test_data_new(n, y, z, 2, r, 'r');
    null_y_roughness_test_data->n_roughness = 2;
    null_y_roughness_test_data->y_roughness = NULL;
    null_y_roughness_test_data->exception = &null_ptr_arg_Error;
    g_test_add("/panthera/xs/crosssection/new/null y_roughness",
               xs_fixture, null_y_roughness_test_data, xs_setup, test_xs_new,
               xs_teardown);
}

void add_xs_hp_tests() {
    g_test_add_func("/panthera/xs/crosssection/hydraulic_properties/"
                    "null arg fail", test_xs_hp_fail);
}

void add_xs_rect_test() {

    int n                        = 5;
    double y[]                   = {0, 0, 0.5, 1, 1};
    double z[]                   = {1, 0, 0, 0, 1};
    double r[]                   = {0.030};
    xs_test_data *rect_test_data = xs_test_data_new(n, y, z, 1, r, 'r');
    rect_test_data->exception = &no_Error;
    g_test_add("/panthera/xs/crosssection/hydraulic_properties/"
               "simple rectangle", xs_fixture, rect_test_data, xs_setup,
               test_simple_h_properties, xs_teardown);
}

void add_xs_triangle_test() {
    int n                            = 5;
    double y[]                       = {0, 0.25, 0.5, 0.75, 1};
    double z[]                       = {1, 0.5, 0, 0.5, 1};
    double r[]                       = {0.030};
    xs_test_data *triangle_test_data = xs_test_data_new(n, y, z, 1, r, 't');
    g_test_add("/panthera/xs/crosssection/hydraulic_properties/"
               "simple triangle", xs_fixture, triangle_test_data, xs_setup,
               test_simple_h_properties, xs_teardown);
}

void add_xs_trapezoid_test() {
    int n                             = 6;
    double y[]                        = {0, 0.25, 0.5, 1.5, 1.75, 2};
    double z[]                        = {1, 0.5, 0, 0, 0.5, 1};
    double r[]                        = {0.030};
    xs_test_data *trapezoid_test_data = xs_test_data_new(n, y, z, 1, r, 'z');
    g_test_add("/panthera/xs/crosssection/hydraulic_properties/"
               "simple trapezoid", xs_fixture, trapezoid_test_data, xs_setup,
               test_simple_h_properties, xs_teardown);
}

void add_xs_double_triangle_test() {
    int n                            = 9;
    double y[]                       = {0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75,
                                        2};
    double z[]                       = {1, 0.5, 0, 0.5, 1, 0.5, 0, 0.5, 1};
    double r[]                       = {0.030, 0.030};
    xs_test_data *triangle_test_data = xs_test_data_new(n, y, z, 2, r, 't');
    triangle_test_data->factor       = 2;
    *(triangle_test_data->y_roughness) = 1;
    g_test_add("/panthera/xs/crosssection/hydraulic_properties/"
               "simple double triangle", xs_fixture, triangle_test_data,
               xs_setup, test_simple_h_properties, xs_teardown);
}

void add_xs_coarray_test() {
    g_test_add_func("/panthera/xs/crosssection/xs_coarray/null arg fail",
                    test_xs_coarray_fail);
}

void add_crosssection_tests() {
    add_xs_new_test();
    add_xs_hp_tests();
    add_xs_rect_test();
    add_xs_triangle_test();
    add_xs_trapezoid_test();
    add_xs_double_triangle_test();
    add_xs_coarray_test();
}
