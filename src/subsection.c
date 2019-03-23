#include "subsection.h"
#include "coarray.h"
#include <stddef.h>

struct Subsection {
    CoArray array;    /* coordinate array */
    double n;         /* Manning's n */
    double min_depth; /* activation depth */
};

Subsection ss_new(CoArray ca, double roughness, double activation_depth) {

    assert((int)(roughness > 0));

    Subsection ss;
    NEW(ss);

    ss->array     = coarray_copy(ca);
    ss->n         = roughness;
    ss->min_depth = activation_depth;

    return ss;
}

void ss_free(Subsection ss) {
    coarray_free(ss->array);
    FREE(ss);
}

double ss_area(Subsection ss, double z) {
    assert(ss);
    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double area       = hp_get_property(hp, HP_AREA);
    hp_free(hp);
    return area;
}

double ss_perimeter(Subsection ss, double z) {
    assert(ss);
    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double perimeter  = hp_get_property(hp, HP_WETTED_PERIMETER);
    hp_free(hp);
    return perimeter;
}

double ss_top_width(Subsection ss, double z) {
    assert(ss);
    HydraulicProps hp = ss_hydraulic_properties(ss, z);
    double width      = hp_get_property(hp, HP_TOP_WIDTH);
    hp_free(hp);
    return width;
}

HydraulicProps ss_hydraulic_properties(Subsection ss, double z) {

    assert(ss);

    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;

    HydraulicProps hp = hp_new();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (z < coarray_min_z(ss->array) || z <= ss->min_depth) {
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

    double y1;
    double z1;

    double y2;
    double z2;

    for (i = 1; i < n; i++) {
        y1 = coarray_get_y(sa, i - 1);
        z1 = coarray_get_z(sa, i - 1);

        y2 = coarray_get_y(sa, i);
        z2 = coarray_get_z(sa, i);

        /* if y1 or y2 is NAN, continue */
        if (isnan(y1) || isnan(y2)) {
            continue;
        }

        /* calculate area by trapezoidal integration */
        d1 = z - z1;
        d2 = z - z2;
        area += 0.5 * (d1 + d2) * (y2 - y1);

        /* calculate perimeter */
        dy = y2 - y1;
        dz = z2 - z1;
        perimeter += sqrt(dy * dy + dz * dz);

        /* calculate top width */
        top_width += y2 - y1;
    }

    hp_set_property(hp, HP_AREA, area);
    hp_set_property(hp, HP_TOP_WIDTH, top_width);
    hp_set_property(hp, HP_WETTED_PERIMETER, perimeter);

    if (sa)
        coarray_free(sa);

    return hp;
}
