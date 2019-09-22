#include "testlib.h"
#include <glib.h>
#include <subsection.h>

#define ABS_TOL 1e-13
#define REL_TOL 0

typedef struct {

    /* initialization data */
    int     n_coords;
    double *y;
    double *z;
    double  roughness;
    double  activation_depth;

    /* shape information */
    /* r - rectangle */
    /* t - triangle */
    /* z - trapezoid */
    char shape;

    /* dimensions for analytical solutions */
    double b0; /* bottom width */
    double s;  /* slope */
    double factor;
} ss_test_data;

ss_test_data *
ss_test_data_new(int n, double *y, double *z, double roughness, char shape)
{

    ss_test_data *test_data = (ss_test_data *) malloc(sizeof(ss_test_data));
    test_data->n_coords     = n;
    test_data->y            = calloc(n, sizeof(double));
    test_data->z            = calloc(n, sizeof(double));
    test_data->shape        = shape;
    test_data->roughness    = roughness;
    test_data->b0           = 1;
    test_data->s            = 0.5;
    test_data->activation_depth = -INFINITY;
    test_data->factor           = 1;

    int i;

    for (i = 0; i < n; i++) {
        *(test_data->y + i) = y[i];
        *(test_data->z + i) = z[i];
    }

    return test_data;
}

void
ss_test_data_free(ss_test_data *test_data)
{
    free(test_data->y);
    free(test_data->z);
    free(test_data);
}

double
calc_area(ss_test_data test_data, double depth)
{
    double area;
    double factor = test_data.factor;

    if (test_data.shape == 'r')
        area = test_data.b0 * depth;
    else if (test_data.shape == 't')
        area = test_data.s * depth * depth;
    else if (test_data.shape == 'z')
        area = (test_data.b0 + test_data.s * depth) * depth;
    else
        g_assert_not_reached();
    return factor * area;
}

double
calc_perimeter(ss_test_data test_data, double depth)
{
    double perimeter;
    double factor = test_data.factor;

    if (depth <= 0)
        perimeter = 0;
    else if (test_data.shape == 'r') {
        perimeter = test_data.b0 + 2 * depth;
    } else if (test_data.shape == 't')
        perimeter = 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else if (test_data.shape == 'z')
        perimeter =
            test_data.b0 + 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else
        g_assert_not_reached();

    return factor * perimeter;
}

double
calc_top_width(ss_test_data test_data, double depth)
{
    double top_width;
    double factor = test_data.factor;

    if (depth <= 0)
        top_width = 0;
    else if (test_data.shape == 'r')
        top_width = test_data.b0;
    else if (test_data.shape == 't')
        top_width = 2 * test_data.s * depth;
    else if (test_data.shape == 'z')
        top_width = test_data.b0 + 2 * test_data.s * depth;
    else
        g_assert_not_reached();

    return factor * top_width;
}

double
calc_hydraulic_radius(ss_test_data test_data, double depth)
{
    double hydraulic_radius;
    double b0 = test_data.b0;
    double s  = test_data.s;

    if (test_data.shape == 'r')
        hydraulic_radius = b0 * depth / (b0 + 2 * depth);
    else if (test_data.shape == 't')
        hydraulic_radius = s * depth / (2 * sqrt(1 + s * s));
    else if (test_data.shape == 'z') {
        double a, b;
        a                = (b0 + s * depth) * depth;
        b                = b0 + 2 * depth * sqrt(1 + s * s);
        hydraulic_radius = a / b;
    } else
        g_assert_not_reached();

    return hydraulic_radius;
}

double
calc_hydraulic_depth(ss_test_data test_data, double depth)
{
    double hydraulic_depth;
    double b0 = test_data.b0;
    double s  = test_data.s;

    if (test_data.shape == 'r')
        hydraulic_depth = depth;
    else if (test_data.shape == 't')
        hydraulic_depth = 0.5 * depth;
    else if (test_data.shape == 'z') {
        double a, b;
        a               = (b0 + s * depth) * depth;
        b               = b0 + 2 * s * depth;
        hydraulic_depth = a / b;
    } else
        g_assert_not_reached();

    return hydraulic_depth;
}

double
calc_conveyance(ss_test_data test_data, double depth)
{
    double area             = calc_area(test_data, depth);
    double hydraulic_radius = calc_hydraulic_radius(test_data, depth);
    double roughness        = test_data.roughness;

    return 1 / roughness * area * pow(hydraulic_radius, 2.0 / 3.0);
}

typedef struct {
    Subsection ss;
} ss_fixture;

void
init_ss(ss_fixture *xsf, ss_test_data test_data)
{
    CoArray ca = coarray_new(test_data.n_coords, test_data.y, test_data.z);
    xsf->ss =
        subsection_new(ca, test_data.roughness, test_data.activation_depth);
    coarray_free(ca);
}

void
ss_setup(ss_fixture *xsf, gconstpointer test_data)
{
    init_ss(xsf, *(const ss_test_data *) test_data);
}

void
ss_teardown(ss_fixture *xsf, gconstpointer test_data)
{
    if (xsf->ss)
        subsection_free(xsf->ss);
    ss_test_data_free((ss_test_data *) test_data);
}

void
test_ss_new(ss_fixture *xsf, gconstpointer test_data)
{
    g_assert_nonnull(xsf->ss);
}

void
check_simple_xsp(ss_fixture *ssf, ss_test_data test_data, double depth)
{
    CrossSectionProps xsp = subsection_properties(ssf->ss, depth);

    bool is_close;

    double calculated_area = xsp_get(xsp, XS_AREA);
    double expected_area   = calc_area(test_data, depth);
    is_close = test_is_close(calculated_area, expected_area, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_tw = xsp_get(xsp, XS_TOP_WIDTH);
    double expected_tw   = calc_top_width(test_data, depth);
    is_close = test_is_close(calculated_tw, expected_tw, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_wp = xsp_get(xsp, XS_WETTED_PERIMETER);
    double expected_wp   = calc_perimeter(test_data, depth);
    is_close = test_is_close(calculated_wp, expected_wp, ABS_TOL, REL_TOL);
    g_assert_true(is_close);

    double calculated_hr = xsp_get(xsp, XS_HYDRAULIC_RADIUS);
    double expected_hr   = calc_hydraulic_radius(test_data, depth);
    if (calculated_area == 0 && calculated_tw == 0)
        g_assert_true(isnan(calculated_hr));
    else {
        is_close = test_is_close(calculated_hr, expected_hr, ABS_TOL, REL_TOL);
        g_assert_true(is_close);
    }

    double calculated_conveyance = xsp_get(xsp, XS_CONVEYANCE);
    double expected_conveyance   = calc_conveyance(test_data, depth);
    if (calculated_area == 0 && calculated_wp == 0) {
        g_assert_true(isnan(calculated_conveyance));
    } else {
        is_close = test_is_close(
            calculated_conveyance, expected_conveyance, ABS_TOL, REL_TOL);
        g_assert_true(is_close);
    }
}

void
test_simple_h_properties(ss_fixture *ssf, gconstpointer test_data)
{

    int    steps = 100;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double) i / (double) steps;
        check_simple_xsp(ssf, *(const ss_test_data *) test_data, depth);
    }
}

void
add_ss_new_test(void)
{
    int    n         = 5;
    double roughness = 0.003;
    double z[]       = { 0, 0, 0.5, 1, 1 };
    double y[]       = { 1, 0, 0, 0, 1 };

    ss_test_data *rect_test_data = ss_test_data_new(n, y, z, roughness, 'r');
    g_test_add("/pollywog/subsection/new",
               ss_fixture,
               rect_test_data,
               ss_setup,
               test_ss_new,
               ss_teardown);
}

void
add_ss_rect_test(void)
{
    int           n              = 5;
    double        z[]            = { 0, 0, 0.5, 1, 1 };
    double        y[]            = { 1, 0, 0, 0, 1 };
    double        roughness      = 0.030;
    ss_test_data *rect_test_data = ss_test_data_new(n, y, z, roughness, 'r');
    g_test_add("/pollywog/subsection/properties/simple rectangle",
               ss_fixture,
               rect_test_data,
               ss_setup,
               test_simple_h_properties,
               ss_teardown);
}

void
add_ss_triangle_test()
{
    int           n         = 5;
    double        z[]       = { 0, 0.25, 0.5, 0.75, 1 };
    double        y[]       = { 1, 0.5, 0, 0.5, 1 };
    double        roughness = 0.030;
    ss_test_data *triangle_test_data =
        ss_test_data_new(n, y, z, roughness, 't');
    g_test_add("/pollywog/subsection/properties/simple triangle",
               ss_fixture,
               triangle_test_data,
               ss_setup,
               test_simple_h_properties,
               ss_teardown);
}

void
add_ss_trapezoid_test()
{
    int           n         = 6;
    double        z[]       = { 0, 0.25, 0.5, 1.5, 1.75, 2 };
    double        y[]       = { 1, 0.5, 0, 0, 0.5, 1 };
    double        roughness = 0.030;
    ss_test_data *trapezoid_test_data =
        ss_test_data_new(n, y, z, roughness, 'z');
    g_test_add("/pollywog/subsection/properties/simple trapezoid",
               ss_fixture,
               trapezoid_test_data,
               ss_setup,
               test_simple_h_properties,
               ss_teardown);
}

void
add_ss_double_triangle_test()
{
    int           n         = 9;
    double        z[]       = { 0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 };
    double        y[]       = { 1, 0.5, 0, 0.5, 1, 0.5, 0, 0.5, 1 };
    double        roughness = 0.030;
    ss_test_data *triangle_test_data =
        ss_test_data_new(n, y, z, roughness, 't');
    triangle_test_data->factor = 2;
    g_test_add("/pollywog/subsection/properties/simple double triangle",
               ss_fixture,
               triangle_test_data,
               ss_setup,
               test_simple_h_properties,
               ss_teardown);
}

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    add_ss_new_test();
    add_ss_rect_test();
    add_ss_triangle_test();
    add_ss_trapezoid_test();
    add_ss_double_triangle_test();

    return g_test_run();
}
