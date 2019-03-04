#include "cii/mem.h"
#include <panthera/xscoordinate.h>

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

void xsc_free(T xsc) { FREE(xsc); }

double xsc_x(T xsc) { return xsc->x; }
double xsc_y(T xsc) { return xsc->y; }
