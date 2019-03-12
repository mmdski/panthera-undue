#ifndef XS_COARRAY_INCLUDED
#define XS_COARRAY_INCLUDED

#include "cii/except.h"
#include "cii/list.h"
#include "coordinate.h"
#include <math.h>

#define T CoArray

/* Except_T raised when coarray_new fails */
extern const Except_T coarray_x_order_Error;

/* Coordinate array type definition */
typedef struct T *T;

/* Allocates memory and creates a new CoArray */
extern T coarray_new(int n, double *x, double *y);

/* Allocates memory and creates an array from an array of Coordinate */
extern T coarray_from_array(int n, Coordinate *array);

/* Allocates memory and creates an array from a List_T */
extern T coarray_from_list(List_T list);

/* Frees the memory from a previously created CoArray */
extern void coarray_free(T a);

/* Returns 1 if a1 and a2 are equal, 0 otherwise */
extern int coarray_eq(T a1, T a2);

/* The number of coordinates in CoArray a */
extern int coarray_length(T a);

/* Gets the i-th coordinate of the CoArray */
Coordinate coarray_get(T a, int i);

/* Returns the minimum y value in the CoArray */
double coarray_min_y(T a);

/* Returns a subarray with points below y-value */
extern T coarray_subarray_y(T a, double y);

#undef T
#endif
