#include "coordinate.h"
#include "cii/mem.h"

#define T Coordinate_T

const Except_T coord_interp_y_Fail = {"Coordinate interpolation failed"};

struct T {
    double x;
    double y;
};

T coord_new(double x, double y) {

    T c;
    NEW(c);

    c->x = x;
    c->y = y;

    return c;
}

T coord_copy(T c) { return coord_new((c->x), (c->y)); }

int coord_eq(T c1, T c2) {
    if (c1 == c2)
        return 1;
    return (coord_x(c1) == coord_x(c2) && coord_y(c1) == coord_y(c2));
}

T coord_interp_y(T c1, T c2, double y) {

    /* raise exception if y is outside fo the range of c1->y and c2->y
      (no extrapolation) */
    if ((y < c1->y && y < c2->y) || (c1->y < y && c2->y < y))
        RAISE(coord_interp_y_Fail);

    double slope = (c2->x - c1->x) / (c2->y - c1->y);
    double x     = slope * (y - c1->y) + c1->x;
    return coord_new(x, y);
}

void coord_free(T c) { FREE(c); }

double coord_x(T c) { return c->x; }
double coord_y(T c) { return c->y; }
