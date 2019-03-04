#ifndef XSCARRAY_INCLUDED
#define XSCARRAY_INCLUDED

#ifndef XSCOORDINATE_INCLUDED
#include <panthera/xscoordinate.h>
#endif

#define T XSCArray_T

typedef struct T *T;

extern T xscarray_new(int n, double *x, double *y);
extern void xscarray_free(T a);
extern int xscarray_n(T a);
extern void xscarray_x(T a, double *x);
extern void xscarray_y(T a, double *y);

#undef T
#endif
