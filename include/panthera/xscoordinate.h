#ifndef XSCOORDINATE_INCLUDED
#define XSCOORDINATE_INCLUDED

#define T XSCoordinate_T

typedef struct T *T;

T xsc_new(double x, double y);
void xsc_free(T s);

double xsc_x(T s);
double xsc_y(T s);

#undef T
#endif
