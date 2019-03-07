#ifndef XSCARRAY_INCLUDED
#define XSCARRAY_INCLUDED

#include "cii/except.h"
#include "coordinate.h"

#define T CoArray_T

extern const Except_T coarray_new_Failed;

typedef struct T *T;

extern T coarray_new(int n, double *x, double *y);
extern void coarray_free(T a);
extern int coarray_n(T a);
extern void coarray_x(T a, double *x);
extern void coarray_y(T a, double *y);

#undef T
#endif
