#ifndef XSCOORDINATE_INCLUDED
#define XSCOORDINATE_INCLUDED

#define T XSCoordinate_T

typedef struct T *T;

T xsc_new(double x, double y);
void xsc_free(T c);

double xsc_x(T c);
double xsc_y(T c);

#undef T
#endif
