#include "cii/mem.h"
#include <panthera/station.h>

#define T Station_T

struct T {
    double x;
    double y;
};

T station_new(double x, double y) {

    T s;
    NEW(s);

    s->x = x;
    s->y = y;

    return s;
}

void station_free(T s) { FREE(s); }

double station_x(T s) { return s->x; }
double station_y(T s) { return s->y; }
