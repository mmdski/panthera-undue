#include "subsection.h"
#include "list.h"
#include "mem.h"
#include <assert.h>
#include <math.h>
#include <panthera/coarray.h>
#include <stddef.h>

/* subsection interface */
struct Subsection {
    CoArray array; /* coordinate array */
    double  n;     /* Manning's n */
    double  min_y; /* activation depth */
};

/* Allocates memory and creates a new Subsection */
Subsection
subsection_new(CoArray ca, double roughness, double activation_depth)
{
    assert((int) (roughness > 0));

    Subsection ss;
    NEW(ss);

    ss->array = coarray_copy(ca);
    ss->n     = roughness;
    ss->min_y = activation_depth;

    return ss;
}

/* Frees memory from a previously allocated Subsection */
void
subsection_free(Subsection ss)
{
    coarray_free(ss->array);
    FREE(ss);
}

/* Calculates hydraulic properties for the subsection.
 * Returns a new CrossSectionProps.
 */
CrossSectionProps
subsection_properties(Subsection ss, double y)
{
    assert(ss);

    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;
    double hydraulic_radius;
    double conveyance;

    CrossSectionProps xsp = xsp_new();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (y <= coarray_min_y(ss->array) || y <= ss->min_y) {
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
    double dy;
    double dz;

    double y1;
    double z1;

    double y2;
    double z2;

    Coordinate c1;
    Coordinate c2;

    for (i = 1; i < n; i++) {

        c1 = coarray_get(sa, i - 1);
        c2 = coarray_get(sa, i);

        if (c1) {
            y1 = c1->y;
            z1 = c1->z;
        } else {
            y1 = NAN;
            z1 = NAN;
        }

        if (c2) {
            y2 = c2->y;
            z2 = c2->z;
        } else {
            y2 = NAN;
            z2 = NAN;
        }

        coord_free(c1);
        coord_free(c2);

        /* if y1 or y2 is NAN, continue */
        if (isnan(y1) || isnan(y2)) {
            continue;
        }

        /* calculate area by trapezoidal integration */
        d1 = y - y1;
        d2 = y - y2;
        area += 0.5 * (d1 + d2) * (z2 - z1);

        /* calculate perimeter */
        dy = y2 - y1;
        dz = z2 - z1;
        perimeter += sqrt(dy * dy + dz * dz);

        /* calculate top width */
        top_width += z2 - z1;
    }

    hydraulic_radius = area / perimeter;
    conveyance       = 1 / ss->n * area * pow(hydraulic_radius, 2.0 / 3.0);

    xsp_set(xsp, XS_AREA, area);
    xsp_set(xsp, XS_TOP_WIDTH, top_width);
    xsp_set(xsp, XS_WETTED_PERIMETER, perimeter);
    xsp_set(xsp, XS_HYDRAULIC_RADIUS, hydraulic_radius);
    xsp_set(xsp, XS_CONVEYANCE, conveyance);

    if (sa)
        coarray_free(sa);

    return xsp;
}

double
subsection_roughness(Subsection ss)
{
    assert(ss);
    return ss->n;
}

bool
subsection_activated(Subsection ss, double y)
{
    assert(ss);
    return (y <= coarray_min_y(ss->array) || y <= ss->min_y);
}

double
subsection_z(Subsection ss)
{
    assert(ss);
    int        n = coarray_length(ss->array);
    double     z;
    Coordinate c = coarray_get(ss->array, n - 1);
    z            = c->z;
    coord_free(c);
    return z;
}
