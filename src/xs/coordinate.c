#include "coordinate.h"
#include "cii/mem.h"

#define T Coordinate_T

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

void coord_free(T c) { FREE(c); }

double coord_x(T c) { return c->x; }
double coord_y(T c) { return c->y; }
