#include "panthera/xscoordinate.h"
#include "cii/mem.h"

#define T XSCoordinate_T

struct T {
    double x;
    double y;
};

T xsc_new(double x, double y) {

    T xsc;
    NEW(xsc);

    xsc->x = x;
    xsc->y = y;

    return xsc;
}

void xsc_free(T c) { FREE(c); }

double xsc_x(T c) { return c->x; }
double xsc_y(T c) { return c->y; }
