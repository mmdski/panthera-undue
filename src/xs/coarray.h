#ifndef XS_COARRAY_INCLUDED
#define XS_COARRAY_INCLUDED

#include "cii/list.h"
#include "coordinate.h"
#include <math.h>
#include "panthera.h"

#define T CoArray

/* Except_T raised when there are too few coordinates passed to an initializer
 */
extern const Except_T coarray_n_coords_Error;

/* Except_T raised when the y-order of coordinates is incorrect */
extern const Except_T coarray_y_order_Error;

/* Coordinate array type definition */
typedef struct T *T;

/* Allocates memory and creates a new CoArray */
extern T coarray_new(int n, double *y, double *z);

/* Allocates memory and returns a copy of CoArray ca */
extern T coarray_copy(CoArray ca);

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

/* Returns a copy of the i-th coordinate of the CoArray,
 * or NULL, if the i-th element is NULL
 */
extern Coordinate coarray_get(T a, int i);

/* Returns the y value of the i-th coordinate, or NAN if the i-th value
 * is NULL.
 */
extern double coarray_get_y(T a, int i);

/* Returns the z value of the i-th coordinate, or NAN if the i-th value
 * is NULL.
 */
extern double coarray_get_z(T a, int i);

/* Returns the minimum z value in the CoArray */
double coarray_min_z(T a);

/* Returns a subarray between ylo and yhi y values (inclusive) */
extern T coarray_subarray_y(T a, double ylo, double yhi);

/* Returns a subarray with points below z value */
extern T coarray_subarray_z(T a, double z);

#undef T
#endif
