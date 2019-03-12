#include "subsection.h"
#include "cii/assert.h"
#include "cii/mem.h"
#include <math.h>
#include <stddef.h>

#define T Subsection

struct T {
    CoArray array;    /* coordinate array */
    double n;         /* Manning's n */
    double min_depth; /* activation depth */
};

static double ss_area(CoArray sa, double y);
static double ss_perimeter(CoArray sa);
static double ss_top_width(CoArray sa);

T subsection_new(int n, double *x, double *y, double roughness,
                 double activation_depth) {

    assert(x);
    assert(y);
    assert((int)(roughness > 0));

    T ss;
    NEW(ss);

    ss->array     = coarray_new(n, x, y);
    ss->n         = roughness;
    ss->min_depth = activation_depth;

    return ss;
}

void subsection_free(T ss) {
    coarray_free(ss->array);
    FREE(ss);
}

double subsection_area(T ss, double y) {

    CoArray sa; /* subarray */

    double area = 0;

    if (y <= ss->min_depth)
        return area;

    sa   = coarray_subarray_y(ss->array, y);
    area = ss_area(sa, y);
    coarray_free(sa);

    return area;
}

double subsection_perimeter(T ss, double y) {
    CoArray sa; /* subarray */

    double perimeter = 0;

    if (y <= ss->min_depth)
        return perimeter;

    sa        = coarray_subarray_y(ss->array, y);
    perimeter = ss_perimeter(sa);
    coarray_free(sa);

    return perimeter;
}

double subsection_top_width(T ss, double y) {

    CoArray sa;

    double width = 0;

    if (y <= ss->min_depth)
        return width;

    sa    = coarray_subarray_y(ss->array, y);
    width = ss_top_width(sa);
    coarray_free(sa);

    return width;
}

HydraulicProps ss_hydraulic_properties(T ss, double y) {

    Coordinate c1;
    Coordinate c2;
    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;

    HydraulicProps hp = hp_new();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (ss->min_depth < y) {
        sa = NULL;
        n  = 0;
    }
    /* otherwise calculate the values */
    else {
        sa = coarray_subarray_y(ss->array, y);
        n  = coarray_length(sa);
    }

    int i;

    /* depth for c1 and c2 */
    double d1;
    double d2;

    /* distances for perimeter */
    double dx;
    double dy;

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        /* calculate area by trapezoidal integration */
        d1 = y - coord_y(c1);
        d2 = y - coord_y(c2);
        area += 0.5 * (d1 + d2) * (coord_x(c2) - coord_x(c1));

        /* calculate perimeter */
        dx = coord_x(c2) - coord_x(c1);
        dy = coord_y(c2) - coord_y(c1);
        perimeter += sqrt(dx * dx + dy * dy);

        /* calculate top width */
        top_width += coord_x(c2) - coord_x(c1);
    }

    hp_set_property(hp, HP_AREA, area);
    hp_set_property(hp, HP_TOP_WIDTH, top_width);
    hp_set_property(hp, HP_WETTED_PERIMETER, perimeter);

    return hp;
}

/* ************************
 * module-level functions *
 * ************************
 */

static double ss_area(CoArray sa, double y) {

    int i; /* for loop */

    Coordinate c1;
    Coordinate c2;

    double d1; /* depth for c1 */
    double d2; /* depth for c2 */

    double area = 0;
    int n       = coarray_length(sa); /* number of coordinates in subarray */

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

static double ss_perimeter(CoArray sa) {

    double perimeter = 0;
    Coordinate c1;
    Coordinate c2;
    double dx; /* distance in x */
    double dy; /* distance in y */

    int i; /* for loop */
    int n = coarray_length(sa);

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

static double ss_top_width(CoArray sa) {

    double width = 0;
    Coordinate c1;
    Coordinate c2;

    int i;
    int n = coarray_length(sa);

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        width += coord_x(c2) - coord_x(c1);
    }

    return width;
}
