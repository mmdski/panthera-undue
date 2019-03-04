#ifndef STATION_INCLUDED
#define STATION_INCLUDED

#define T Station_T

typedef struct T *T;

T station_new(double x, double y);
void station_free(T s);

double station_x(T s);
double station_y(T s);

#undef T
#endif
