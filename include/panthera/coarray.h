#ifndef COARRAY_INCLUDED
#define COARRAY_INCLUDED

#include "panthera.h"

/*  Exception raised when there are too few coordinates passed to an
 *  initializer
 */
extern const Except_T coarray_n_coords_Error;

/*  Exception raised when the y-order of coordinates is incorrect
 */
extern const Except_T coarray_y_order_Error;

/*  Coordinate array type definition
 */
typedef struct CoArray *CoArray;

/*  Allocates memory and creates a new CoArray
 */
extern CoArray coarray_new(int n, double *y, double *z);

/*  Allocates memory and returns a copy of CoArray ca
 */
extern CoArray coarray_copy(CoArray ca);

/*  Frees the memory from a previously created CoArray
 */
extern void coarray_free(CoArray a);

/*  Returns 1 if a1 and a2 are equal, 0 otherwise
 */
extern int coarray_eq(CoArray a1, CoArray a2);

/*  The number of coordinates in CoArray a
 */
extern int coarray_length(CoArray a);

/*  Returns the y value of the i-th coordinate, or NAN if the i-th value
 *  is NULL.
 */
extern double coarray_get_y(CoArray a, int i);

/*  Returns the z value of the i-th coordinate, or NAN if the i-th value
 *  is NULL.
 */
extern double coarray_get_z(CoArray a, int i);

/*  Returns the minimum z value in the CoArray
 */
double coarray_min_z(CoArray a);

/*  Returns a subarray between ylo and yhi y values (inclusive)
 */
extern CoArray coarray_subarray_y(CoArray a, double ylo, double yhi);

/*  Returns a subarray with points below z value
 */
extern CoArray coarray_subarray_z(CoArray a, double z);

/*  Returns a CoArray z-translated by subtract-z
 */
extern CoArray coarray_add_z(CoArray ca, double add_z);

#endif
