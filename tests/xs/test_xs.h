#ifndef TEST_XS_INCLUDED
#define TEST_XS_INCLUDED

#include "testlib.h"

typedef struct {

    /* initialization data */
    int n_coords;
    double *y;
    double *z;
    double n_roughness;
    double *roughness;
    double *y_roughness;
    double activation_depth;

    /* shape information */
    /* r - rectangle */
    /* t - triangle */
    /* z - trapezoid */
    char shape;

    /* dimensions for analytical solutions */
    double b0; /* bottom width */
    double s;  /* slope */
} xs_test_data;

extern xs_test_data *xs_test_data_new(int n, double *y, double *z, char shape);
extern void xs_test_data_free(xs_test_data *test_data);

extern double calc_area(xs_test_data test_data, double depth);
extern double calc_perimeter(xs_test_data test_data, double depth);
extern double calc_top_width(xs_test_data test_data, double depth);
extern double calc_hydraulic_depth(xs_test_data test_data, double depth);
extern double calc_hydraulic_radius(xs_test_data test_data, double depth);

#endif
