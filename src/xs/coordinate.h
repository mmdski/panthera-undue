#ifndef XS_COORDINATE_INCLUDED
#define XS_COORDINATE_INCLUDED

#define T Coordinate_T

typedef struct T *T;

T coord_new(double x, double y);
void coord_free(T c);

double coord_x(T c);
double coord_y(T c);

#undef T
#endif
