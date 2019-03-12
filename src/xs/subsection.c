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

T ss_new(int n, double *x, double *y, double roughness,
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

void ss_free(T ss) {
    coarray_free(ss->array);
    FREE(ss);
}

double ss_area(T ss, double y) {

    HydraulicProps hp = ss_hydraulic_properties(ss, y);
    double area       = hp_get_property(hp, HP_AREA);
    hp_free(hp);
    return area;
}

double ss_perimeter(T ss, double y) {
    HydraulicProps hp = ss_hydraulic_properties(ss, y);
    double perimeter  = hp_get_property(hp, HP_WETTED_PERIMETER);
    hp_free(hp);
    return perimeter;
}

double ss_top_width(T ss, double y) {
    HydraulicProps hp = ss_hydraulic_properties(ss, y);
    double width      = hp_get_property(hp, HP_TOP_WIDTH);
    hp_free(hp);
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
    if (y <= ss->min_depth) {
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

    if (sa)
        coarray_free(sa);

    return hp;
}
