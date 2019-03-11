#include "subsection.h"
#include "cii/assert.h"
#include "cii/mem.h"
#include <math.h>
#include <stddef.h>

#define T Subsection_T

struct T {
    CoArray_T a; /* coordinate array */
    double n;    /* Manning's n */
    double d;    /* activation depth */
};

static double ss_area(CoArray_T sa, double y);
static double ss_perimeter(CoArray_T sa);
static double ss_top_width(CoArray_T sa);

T subsection_new(int n, double *x, double *y, double roughness,
                 double activation_depth) {

    assert(x);
    assert(y);
    assert((int)(roughness > 0));

    T ss;
    NEW(ss);

    ss->a = coarray_new(n, x, y);
    ss->n = roughness;
    ss->d = activation_depth;

    return ss;
}

void subsection_free(T ss) {
    coarray_free(ss->a);
    FREE(ss);
}

double subsection_area(T ss, double y) {

    CoArray_T sa; /* subarray */

    double area = 0;

    if (y <= ss->d)
        return area;

    sa   = coarray_subarray_y(ss->a, y);
    area = ss_area(sa, y);
    coarray_free(sa);

    return area;
}

double subsection_perimeter(T ss, double y) {
    CoArray_T sa; /* subarray */

    double perimeter = 0;

    if (y <= ss->d)
        return perimeter;

    sa        = coarray_subarray_y(ss->a, y);
    perimeter = ss_perimeter(sa);
    coarray_free(sa);

    return perimeter;
}

double subsection_top_width(T ss, double y) {

    CoArray_T sa;

    double width = 0;

    if (y <= ss->d)
        return width;

    sa    = coarray_subarray_y(ss->a, y);
    width = ss_top_width(sa);
    coarray_free(sa);

    return width;
}

/* ************************
 * module-level functions *
 * ************************
 */

static double ss_area(CoArray_T sa, double y) {

    int i; /* for loop */

    Coordinate_T c1;
    Coordinate_T c2;

    double d1; /* depth for c1 */
    double d2; /* depth for c2 */

    double area = 0;
    int n       = coarray_n(sa); /* number of coordinates in subarray */

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        d1 = y - coord_y(c1);
        d2 = y - coord_y(c2);

        /* trapezoidal integration */
        area += 0.5 * (d1 + d2) * (coord_x(c2) - coord_x(c1));
    }

    return area;
}

static double ss_perimeter(CoArray_T sa) {

    double perimeter = 0;
    Coordinate_T c1;
    Coordinate_T c2;
    double dx; /* distance in x */
    double dy; /* distance in y */

    int i; /* for loop */
    int n = coarray_n(sa);

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        dx = coord_x(c2) - coord_x(c1);
        dy = coord_y(c2) - coord_y(c1);

        perimeter += sqrt(dx * dx + dy * dy);
    }

    return perimeter;
}

static double ss_top_width(CoArray_T sa) {

    double width = 0;
    Coordinate_T c1;
    Coordinate_T c2;

    int i;
    int n = coarray_n(sa);

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        width += coord_x(c2) - coord_x(c1);
    }

    return width;
}
