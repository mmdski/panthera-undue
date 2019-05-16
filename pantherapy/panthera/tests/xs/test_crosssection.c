#include "testlib.h"
#include <glib.h>
#include <panthera/crosssection.h>

#define ABS_TOL 1e-14
#define REL_TOL 0

typedef struct {

    /* initialization data */
    int     n_coords;
    double *y;
    double *z;
    double  n_roughness;
    double *roughness;
    double *z_roughness;
    double  activation_depth;

    /* shape information */
    /* r - rectangle */
    /* t - triangle */
    /* z - trapezoid */
    char shape;

    /* dimensions for analytical solutions */
    double          b0; /* bottom width */
    double          s;  /* slope */
    const Except_T *exception;
    double          factor;
} xs_test_data;

xs_test_data *
xs_test_data_new (int     n,
                  double *y,
                  double *z,
                  int     n_roughness,
                  double *roughness,
                  char    shape)
{

    xs_test_data *test_data =
        (xs_test_data *) Mem_alloc (sizeof (xs_test_data), __FILE__, __LINE__);
    test_data->n_coords = n;
    test_data->y        = Mem_calloc (n, sizeof (double), __FILE__, __LINE__);
    test_data->z        = Mem_calloc (n, sizeof (double), __FILE__, __LINE__);
    test_data->shape    = shape;
    test_data->n_roughness = n_roughness;
    test_data->roughness   = Mem_calloc (
        test_data->n_roughness, sizeof (double), __FILE__, __LINE__);
    if (n_roughness > 1)
        test_data->z_roughness = Mem_calloc (
            test_data->n_roughness, sizeof (double), __FILE__, __LINE__);
    else
        test_data->z_roughness = NULL;
    test_data->b0               = 1;
    test_data->s                = 0.5;
    test_data->activation_depth = -INFINITY;
    test_data->factor           = 1;

    int i;

    for (i = 0; i < n; i++) {
        *(test_data->y + i) = y[i];
        *(test_data->z + i) = z[i];
    }

    for (i = 0; i < n_roughness; i++) {
        *(test_data->roughness + i) = roughness[i];
    }

    return test_data;
}

void
xs_test_data_free (xs_test_data *test_data)
{
    Mem_free (test_data->y, __FILE__, __LINE__);
    Mem_free (test_data->z, __FILE__, __LINE__);
    Mem_free (test_data->z_roughness, __FILE__, __LINE__);
    Mem_free (test_data->roughness, __FILE__, __LINE__);
    Mem_free (test_data, __FILE__, __LINE__);
}

double
calc_area (xs_test_data test_data, double depth)
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
        g_assert_not_reached ();
    return factor * area;
}

double
calc_perimeter (xs_test_data test_data, double depth)
{
    double perimeter;
    double factor = test_data.factor;

    if (test_data.shape == 'r') {
        perimeter = test_data.b0 + 2 * depth;
    } else if (test_data.shape == 't')
        perimeter = 2 * depth * sqrt (1 + (test_data.s * test_data.s));
    else if (test_data.shape == 'z')
        perimeter =
            test_data.b0 + 2 * depth * sqrt (1 + (test_data.s * test_data.s));
    else
        g_assert_not_reached ();

    return factor * perimeter;
}

double
calc_top_width (xs_test_data test_data, double depth)
{
    double top_width;
    double factor = test_data.factor;

    if (test_data.shape == 'r')
        top_width = test_data.b0;
    else if (test_data.shape == 't')
        top_width = 2 * test_data.s * depth;
    else if (test_data.shape == 'z')
        top_width = test_data.b0 + 2 * test_data.s * depth;
    else
        g_assert_not_reached ();

    return factor * top_width;
}

double
calc_hydraulic_radius (xs_test_data test_data, double depth)
{
    double hydraulic_radius;
    double b0 = test_data.b0;
    double s  = test_data.s;

    if (test_data.shape == 'r')
        hydraulic_radius = b0 * depth / (b0 + 2 * depth);
    else if (test_data.shape == 't')
        hydraulic_radius = s * depth / (2 * sqrt (1 + s * s));
    else if (test_data.shape == 'z') {
        double a, b;
        a                = (b0 + s * depth) * depth;
        b                = b0 + 2 * depth * sqrt (1 + s * s);
        hydraulic_radius = a / b;
    } else
        g_assert_not_reached ();

    return hydraulic_radius;
}

double
calc_hydraulic_depth (xs_test_data test_data, double depth)
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
        g_assert_not_reached ();

    return hydraulic_depth;
}

double
calc_conveyance (xs_test_data test_data, double depth)
{

    /* only works for non-compound roughness cases */
    g_assert_true (test_data.n_roughness == 1);

    double area             = calc_area (test_data, depth);
    double hydraulic_radius = calc_hydraulic_radius (test_data, depth);
    double roughness        = *test_data.roughness;

    return 1 / roughness * area * pow (hydraulic_radius, 2.0 / 3.0);
}

typedef struct {
    CrossSection    xs;
    const Except_T *caught_exception;
} xs_fixture;

void
init_xs (xs_fixture *xsf, xs_test_data test_data)
{
    TRY CoArray ca =
        coarray_new (test_data.n_coords, test_data.y, test_data.z);
    xsf->xs = xs_new (
        ca, test_data.n_roughness, test_data.roughness, test_data.z_roughness);
    coarray_free (ca);
    xsf->caught_exception = &no_error;
    EXCEPT (null_ptr_arg_error)
    xsf->caught_exception = &null_ptr_arg_error;
    EXCEPT (value_arg_error)
    xsf->caught_exception = &value_arg_error;
    END_TRY;
}

void
xs_setup (xs_fixture *xsf, gconstpointer test_data)
{
    init_xs (xsf, *(const xs_test_data *) test_data);
}

void
xs_teardown (xs_fixture *xsf, gconstpointer test_data)
{
    if (xsf->xs)
        xs_free (xsf->xs);
    xs_test_data_free ((xs_test_data *) test_data);
}

void
test_xs_new (xs_fixture *xsf, gconstpointer test_data)
{
    xs_test_data data = *(const xs_test_data *) test_data;
    g_assert_true (xsf->caught_exception == data.exception);
}

void
check_simple_xsp (xs_fixture *xsf, xs_test_data test_data, double depth)
{
    CrossSectionProps xsp = xs_hydraulic_properties (xsf->xs, depth);

    bool is_close;

    double calculated_area = xsp_get (xsp, XS_AREA);
    double expected_area   = calc_area (test_data, depth);
    is_close =
        test_is_close (calculated_area, expected_area, ABS_TOL, REL_TOL);
    g_assert_true (is_close);

    double calculated_tw = xsp_get (xsp, XS_TOP_WIDTH);
    double expected_tw   = calc_top_width (test_data, depth);
    is_close = test_is_close (calculated_tw, expected_tw, ABS_TOL, REL_TOL);
    g_assert_true (is_close);

    double calculated_wp = xsp_get (xsp, XS_WETTED_PERIMETER);
    double expected_wp   = calc_perimeter (test_data, depth);
    is_close = test_is_close (calculated_wp, expected_wp, ABS_TOL, REL_TOL);
    g_assert_true (is_close);

    double calculated_hd = xsp_get (xsp, XS_HYDRAULIC_DEPTH);
    double expected_hd   = calc_hydraulic_depth (test_data, depth);
    is_close = test_is_close (calculated_hd, expected_hd, ABS_TOL, REL_TOL);
    g_assert_true (is_close);

    double calculated_hr = xsp_get (xsp, XS_HYDRAULIC_RADIUS);
    double expected_hr   = calc_hydraulic_radius (test_data, depth);
    is_close = test_is_close (calculated_hr, expected_hr, ABS_TOL, REL_TOL);
    g_assert_true (is_close);

    /* only test cross sections that contain a single subsection */
    if (test_data.n_roughness == 1) {
        double calculated_conveyance = xsp_get (xsp, XS_CONVEYANCE);
        double expected_conveyance   = calc_conveyance (test_data, depth);
        if (calculated_area == 0 && calculated_wp == 0) {
            g_assert_true (isnan (calculated_conveyance));
        } else {
            is_close = test_is_close (
                calculated_conveyance, expected_conveyance, ABS_TOL, REL_TOL);
            g_assert_true (is_close);
        }

        double calculated_velocity_coeff = xsp_get (xsp, XS_VELOCITY_COEFF);
        double expected_velocity_coeff   = 1;
        is_close = test_is_close (calculated_velocity_coeff,
                                  expected_velocity_coeff,
                                  ABS_TOL,
                                  REL_TOL);
        if (depth > 0)
            g_assert_true (is_close);
    }
}

void
test_simple_h_properties (xs_fixture *xsf, gconstpointer test_data)
{

    int    steps = 10;
    double depth;
    for (int i = 0; i < steps; i++) {
        depth = (double) i / (double) steps;
        check_simple_xsp (xsf, *(const xs_test_data *) test_data, depth);
    }
}

void
test_xs_xsp_null_fail (void)
{
    TRY xs_hydraulic_properties (NULL, 1);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    END_TRY;
}

void
test_xsp_depth_error (void)
{
    CrossSection      xs = new_cross_section ();
    CrossSectionProps xsp;

    TRY xsp = xs_hydraulic_properties (xs, -1.);
    xsp_free (xsp);
    g_assert_not_reached ();
    EXCEPT (xsp_depth_error);
    ;
    END_TRY;
}

void
test_xs_coarray_fail (void)
{
    TRY xs_coarray (NULL);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    END_TRY;
}

void
add_xs_new_test ()
{
    int    n        = 5;
    double z[]      = { 0, 0, 0.5, 1, 1 };
    double y[]      = { 1, 0, 0, 0, 1 };
    double r[]      = { 0.030 };
    double zero_r[] = { 0 };
    double neg_r[]  = { -0.030 };

    xs_test_data *rect_test_data = xs_test_data_new (n, y, z, 1, r, 'r');
    rect_test_data->exception    = &no_error;
    g_test_add ("/panthera/xs/crosssection/new/no error",
                xs_fixture,
                rect_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);

    xs_test_data *zero_n_roughness_test_data =
        xs_test_data_new (n, y, z, 1, r, 'r');
    zero_n_roughness_test_data->n_roughness = 0;
    zero_n_roughness_test_data->exception   = &value_arg_error;
    g_test_add ("/panthera/xs/crosssection/new/zero n_roughness",
                xs_fixture,
                zero_n_roughness_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);

    xs_test_data *zero_roughness_test_data =
        xs_test_data_new (n, y, z, 1, zero_r, 'r');
    zero_roughness_test_data->exception = &value_arg_error;
    g_test_add ("/panthera/xs/crosssection/new/zero roughness",
                xs_fixture,
                zero_roughness_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);

    xs_test_data *neg_roughness_test_data =
        xs_test_data_new (n, y, z, 1, neg_r, 'r');
    neg_roughness_test_data->exception = &value_arg_error;
    g_test_add ("/panthera/xs/crosssection/new/negative roughness",
                xs_fixture,
                neg_roughness_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);

    xs_test_data *null_roughness_test_data =
        xs_test_data_new (n, y, z, 1, r, 'r');
    null_roughness_test_data->roughness = NULL;
    null_roughness_test_data->exception = &null_ptr_arg_error;
    g_test_add ("/panthera/xs/crosssection/new/null roughness",
                xs_fixture,
                null_roughness_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);

    xs_test_data *null_y_roughness_test_data =
        xs_test_data_new (n, y, z, 2, r, 'r');
    null_y_roughness_test_data->n_roughness = 2;
    null_y_roughness_test_data->z_roughness = NULL;
    null_y_roughness_test_data->exception   = &null_ptr_arg_error;
    g_test_add ("/panthera/xs/crosssection/new/null z_roughness",
                xs_fixture,
                null_y_roughness_test_data,
                xs_setup,
                test_xs_new,
                xs_teardown);
}

void
add_xs_xsp_tests ()
{
    g_test_add_func ("/panthera/xs/crosssection/hydraulic_properties/"
                     "null arg fail",
                     test_xs_xsp_null_fail);
    g_test_add_func ("/panthera/xs/crosssection/hydraulic_properties/"
                     "depth error",
                     test_xsp_depth_error);
}

void
add_xs_rect_test ()
{

    int           n              = 5;
    double        z[]            = { 0, 0, 0.5, 1, 1 };
    double        y[]            = { 1, 0, 0, 0, 1 };
    double        r[]            = { 0.030 };
    xs_test_data *rect_test_data = xs_test_data_new (n, y, z, 1, r, 'r');
    rect_test_data->exception    = &no_error;
    g_test_add ("/panthera/xs/crosssection/hydraulic_properties/"
                "simple rectangle",
                xs_fixture,
                rect_test_data,
                xs_setup,
                test_simple_h_properties,
                xs_teardown);
}

void
add_xs_triangle_test ()
{
    int           n                  = 5;
    double        z[]                = { 0, 0.25, 0.5, 0.75, 1 };
    double        y[]                = { 1, 0.5, 0, 0.5, 1 };
    double        r[]                = { 0.030 };
    xs_test_data *triangle_test_data = xs_test_data_new (n, y, z, 1, r, 't');
    g_test_add ("/panthera/xs/crosssection/hydraulic_properties/"
                "simple triangle",
                xs_fixture,
                triangle_test_data,
                xs_setup,
                test_simple_h_properties,
                xs_teardown);
}

void
add_xs_trapezoid_test ()
{
    int           n                   = 6;
    double        z[]                 = { 0, 0.25, 0.5, 1.5, 1.75, 2 };
    double        y[]                 = { 1, 0.5, 0, 0, 0.5, 1 };
    double        r[]                 = { 0.030 };
    xs_test_data *trapezoid_test_data = xs_test_data_new (n, y, z, 1, r, 'z');
    g_test_add ("/panthera/xs/crosssection/hydraulic_properties/"
                "simple trapezoid",
                xs_fixture,
                trapezoid_test_data,
                xs_setup,
                test_simple_h_properties,
                xs_teardown);
}

void
add_xs_double_triangle_test ()
{
    int           n   = 9;
    double        z[] = { 0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 };
    double        y[] = { 1, 0.5, 0, 0.5, 1, 0.5, 0, 0.5, 1 };
    double        r[] = { 0.030, 0.030 };
    xs_test_data *triangle_test_data   = xs_test_data_new (n, y, z, 2, r, 't');
    triangle_test_data->factor         = 2;
    *(triangle_test_data->z_roughness) = 1;
    g_test_add ("/panthera/xs/crosssection/hydraulic_properties/"
                "simple double triangle",
                xs_fixture,
                triangle_test_data,
                xs_setup,
                test_simple_h_properties,
                xs_teardown);
}

void
add_xs_coarray_test ()
{
    g_test_add_func ("/panthera/xs/crosssection/xs_coarray/null arg fail",
                     test_xs_coarray_fail);
}

void
add_crosssection_tests ()
{
    add_xs_new_test ();
    add_xs_xsp_tests ();
    add_xs_rect_test ();
    add_xs_triangle_test ();
    add_xs_trapezoid_test ();
    add_xs_double_triangle_test ();
    add_xs_coarray_test ();
}
