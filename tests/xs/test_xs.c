#include "test_xs.h"

extern void add_coord_tests(void);
extern void add_coarray_tests(void);
extern void add_subsection_tests(void);
extern void add_crosssection_tests(void);

void add_xs_tests(void) {
    add_coord_tests();
    add_coarray_tests();
    add_subsection_tests();
    add_crosssection_tests();
}

xs_test_data *xs_test_data_new(int n, double *y, double *z, int n_roughness,
                               double *roughness, char shape) {

    xs_test_data *test_data =
        (xs_test_data *)Mem_alloc(sizeof(xs_test_data), __FILE__, __LINE__);
    test_data->n_coords    = n;
    test_data->y           = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    test_data->z           = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    test_data->shape       = shape;
    test_data->n_roughness = n_roughness;
    test_data->roughness   = Mem_calloc(test_data->n_roughness, sizeof(double),
                                        __FILE__, __LINE__);
    if (n_roughness > 1)
        test_data->y_roughness  = Mem_calloc(test_data->n_roughness,
                                             sizeof(double), __FILE__,
                                             __LINE__);
    else
        test_data->y_roughness = NULL;
    test_data->b0          = 1;
    test_data->s           = 0.5;
    test_data->activation_depth = -INFINITY;
    test_data->factor      = 1;

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

void xs_test_data_free(xs_test_data *test_data) {
    Mem_free(test_data->y, __FILE__, __LINE__);
    Mem_free(test_data->z, __FILE__, __LINE__);
    Mem_free(test_data->y_roughness, __FILE__, __LINE__);
    Mem_free(test_data->roughness, __FILE__, __LINE__);
    Mem_free(test_data, __FILE__, __LINE__);
}

double calc_area(xs_test_data test_data, double depth) {
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
    return factor*area;
}

double calc_perimeter(xs_test_data test_data, double depth) {
    double perimeter;
    double factor = test_data.factor;

    if (test_data.shape == 'r') {
        perimeter = test_data.b0 + 2 * depth;
    } else if (test_data.shape == 't')
        perimeter = 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else if (test_data.shape == 'z')
        perimeter =
            test_data.b0 + 2 * depth * sqrt(1 + (test_data.s * test_data.s));
    else
        g_assert_not_reached();

    return factor*perimeter;
}

double calc_top_width(xs_test_data test_data, double depth) {
    double top_width;
    double factor = test_data.factor;

    if (test_data.shape == 'r')
        top_width = test_data.b0;
    else if (test_data.shape == 't')
        top_width = 2 * test_data.s * depth;
    else if (test_data.shape == 'z')
        top_width = test_data.b0 + 2 * test_data.s * depth;
    else
        g_assert_not_reached();

    return factor*top_width;
}

double calc_hydraulic_radius(xs_test_data test_data, double depth) {
    double hydraulic_radius;
    double b0 = test_data.b0;
    double s = test_data.s;

    if (test_data.shape == 'r')
        hydraulic_radius = b0*depth/(b0 + 2*depth);
    else if (test_data.shape == 't')
        hydraulic_radius = s*depth/(2*sqrt(1 + s*s));
    else if (test_data.shape == 'z') {
        double a, b;
        a = (b0 + s*depth) * depth;
        b = b0 + 2*depth*sqrt(1 + s*s);
        hydraulic_radius = a/b;
    } else
        g_assert_not_reached();

    return hydraulic_radius;
}

double calc_hydraulic_depth(xs_test_data test_data, double depth) {
    double hydraulic_depth;
    double b0 = test_data.b0;
    double s = test_data.s;

    if (test_data.shape == 'r')
        hydraulic_depth = depth;
    else if (test_data.shape == 't')
        hydraulic_depth = 0.5*depth;
    else if (test_data.shape == 'z') {
        double a, b;
        a = (b0 + s*depth)*depth;
        b = b0 + 2*s*depth;
        hydraulic_depth = a/b;
    } else
        g_assert_not_reached();

    return hydraulic_depth;
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);
    add_xs_tests();
    return g_test_run();
}
