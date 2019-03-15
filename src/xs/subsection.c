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

T ss_new(int n, double *y, double *z, double roughness,
         double activation_depth) {

    assert(y);
    assert(z);
    assert((int)(roughness > 0));

    T ss;
    NEW(ss);

    ss->array     = coarray_new(n, y, z);
    ss->n         = roughness;
    ss->min_depth = activation_depth;

    return ss;
}

void ss_free(T ss) {
    coarray_free(ss->array);
    FREE(ss);
}

double ss_area(T ss, double z) {

    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double area       = hp_get_property(hp, HP_AREA);
    hp_free(hp);
    return area;
}

double ss_perimeter(T ss, double z) {
    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double perimeter  = hp_get_property(hp, HP_WETTED_PERIMETER);
    hp_free(hp);
    return perimeter;
}

double ss_top_width(T ss, double z) {
    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double width      = hp_get_property(hp, HP_TOP_WIDTH);
    hp_free(hp);
    return width;
}

HydraulicProps ss_hydraulic_properties(T ss, double z) {

    Coordinate c1;
    Coordinate c2;
    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;

    HydraulicProps hp = hp_new();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (z <= coarray_min_z(ss->array) || z <= ss->min_depth) {
        sa = NULL;
        n  = 0;
    }
    /* otherwise calculate the values */
    else {
        sa = coarray_subarray_z(ss->array, z);
        n  = coarray_length(sa);
    }

    int i;

    /* depth for c1 and c2 */
    double d1;
    double d2;

    /* distances for perimeter */
    double dy;
    double dz;

    for (i = 1; i < n; i++) {
        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1 == NULL || c2 == NULL)
            continue;

        /* calculate area by trapezoidal integration */
        d1 = z - coord_z(c1);
        d2 = z - coord_z(c2);
        area += 0.5 * (d1 + d2) * (coord_y(c2) - coord_y(c1));

        /* calculate perimeter */
        dy = coord_y(c2) - coord_y(c1);
        dz = coord_z(c2) - coord_z(c1);
        perimeter += sqrt(dy * dy + dz * dz);

        /* calculate top width */
        top_width += coord_y(c2) - coord_y(c1);
    }

    hp_set_property(hp, HP_AREA, area);
    hp_set_property(hp, HP_TOP_WIDTH, top_width);
    hp_set_property(hp, HP_WETTED_PERIMETER, perimeter);

    if (sa)
        coarray_free(sa);

    return hp;
}
