#ifndef XS_COARRAY_INCLUDED
#define XS_COARRAY_INCLUDED

#include "cii/except.h"
#include "coordinate.h"

#define T CoArray_T

/* Except_T raised when coarray_new fails */
extern const Except_T coarray_new_Failed;

/* Coordinate array type definition */
typedef struct T *T;

/* Allocate memory and create a new CoArray_T */
extern T coarray_new(int n, double *x, double *y);

/* Free the memory from a previously created CoArray_T */
extern void coarray_free(T a);

/* Return 1 if a1 and a2 are equal, 0 otherwise */
extern int coarray_eq(T a1, T a2);

/* The number of coordinates in CoArray_T a */
extern int coarray_n(T a);

/* Get the i-th coordinate of the CoArray_T */
Coordinate_T coarray_get(T a, int i);

/* Returns a subarray with points below y-value */
extern T coarray_subarray_y(T a, double y);

#undef T
#endif
