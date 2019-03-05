#ifndef XSCARRAY_INCLUDED
#define XSCARRAY_INCLUDED

#include "cii/except.h"
#include <panthera/xscoordinate.h>

#define T XSCArray_T

extern const Except_T xscarray_new_Failed;

typedef struct T *T;

extern T xscarray_new(int n, double *x, double *y);
extern void xscarray_free(T a);
extern int xscarray_n(T a);
extern void xscarray_x(T a, double *x);
extern void xscarray_y(T a, double *y);

#undef T
#endif
