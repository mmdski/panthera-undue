#include "coordinate.h"
#include "cii/mem.h"

#define T Coordinate

const Except_T coord_interp_Fail = {"Coordinate interpolation failed"};

struct T {
    double y; /* lateral coordinate */
    double z; /* vertical coordinate */
};

T coord_new(double y, double z) {

    T c;
    NEW(c);

    c->y = y;
    c->z = z;

    return c;
}

T coord_copy(T c) { return coord_new((c->y), (c->z)); }

void coord_free(T c) { FREE(c); }

int coord_eq(T c1, T c2) {
    if (c1 == c2)
        return 1;
    return (coord_y(c1) == coord_y(c2) && coord_z(c1) == coord_z(c2));
}

T coord_interp_y(T c1, T c2, double y) {

    /* raise exception if y is outside fo the range of c1->y and c2->y
      (no extrapolation) */
    if ((y < c1->y && y < c2->y) || (c1->y < y && c2->y < y))
        RAISE(coord_interp_Fail);

    double slope = (c2->z - c1->z) / (c2->y - c1->y);
    double z     = slope * (y - c1->y) + c1->z;
    return coord_new(y, z);
}

T coord_interp_z(T c1, T c2, double z) {

    /* raise exception if z is outside fo the range of c1->z and c2->z
      (no extrapolation) */
    if ((z < c1->z && z < c2->z) || (c1->z < z && c2->z < z))
        RAISE(coord_interp_Fail);

    double slope = (c2->y - c1->y) / (c2->z - c1->z);
    double y     = slope * (z - c1->z) + c1->y;
    return coord_new(y, z);
}

double coord_y(T c) { return c->y; }
double coord_z(T c) { return c->z; }
