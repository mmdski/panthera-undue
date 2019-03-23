#ifndef COARRAY_INCLUDED
#define COARRAY_INCLUDED

#include "panthera.h"

/**
 * coarray_n_coords_Error:
 *
 * Exception raised when there are too few coordinates passed to coarray_new()
 */
extern const Except_T coarray_n_coords_Error;

/**
 * coarray_y_order_Error:
 *
 * Exception raised when the order of @y passed to coarray_new() is incorrect
 */
extern const Except_T coarray_y_order_Error;

/**
 * coarray_index_Error:
 *
 * Exception raised when an invalid index value is used to retrieve array
 * values.
 */
extern const Except_T coarray_index_Error;

/*  Coordinate array type definition
 */
typedef struct CoArray *CoArray;

/**
 * coarray_new:
 * @n: the length of @y and @z
 * @y: pointer to an array of @n y-values
 * @z: pointer to an array of @n z-values
 *
 * Creates a new coordinate array with length @n and y- and z-values of @y and
 * @z. @n must be greater than 2. This function raises #coarray_n_coords_Error
 * otherwise. Each value of @y beyond the first element must be greater than or
 * equal to the previous element. This function raises #coarray_y_order_Error
 * otherwise. The resulting coordinate array is newly allocated and must be
 * freed with coarray_free().
 *
 * Returns: a new #CoArray
 */
extern CoArray coarray_new(int n, double *y, double *z);

/**
 * coarray_copy:
 * @ca: a #CoArray
 *
 * Returns a new copy of @ca. The returned #CoArray is newly allocated and
 * should be freed using coarray_free().
 *
 * Returns: a copy of @ca
 */
extern CoArray coarray_copy(CoArray ca);

/**
 * coarray_free:
 * @a: a #CoArray
 *
 * Frees a coordinate array.
 */
extern void coarray_free(CoArray a);

/**
 * coarray_eq:
 * @a1: a #CoArray
 * @a2: another #CoArray
 *
 * Returns: 1 if a1 and a2 are equal, 0 if they are not.
 */
extern int coarray_eq(CoArray a1, CoArray a2);

/**
 * coarray_length:
 * @a: a #CoArray
 *
 * Returns: the length of @a.
 */
extern int coarray_length(CoArray a);

/**
 * coarray_get_y:
 * @a: a #CoArray
 * @i: index value
 *
 * Returns y-value of the `i`-th coordinate of an array. If `i < 0` or
 * `i >= length`, where `length` is the length of @a, this function raises
 * #coarray_index_Error. Use coarray_length() to get the length of @a.
 *
 * Returns: y-value of `i`-th coordinate
 */
extern double coarray_get_y(CoArray a, int i);

/**
 * coarray_get_z:
 * @a: a #CoArray
 * @i: index value
 *
 * Returns z-value of the `i`-th coordinate of an array. If `i < 0` or
 * `i >= length`, where `length` is the length of @a, this function raises
 * #coarray_index_Error. Use coarray_length() to get the length of @a.
 *
 * Returns: z-value of `i`-th coordinate
 */
extern double coarray_get_z(CoArray a, int i);

#endif
