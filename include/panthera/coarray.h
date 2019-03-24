#ifndef COARRAY_INCLUDED
#define COARRAY_INCLUDED

#include <cii/mem.h>
#include <panthera/exceptions.h>
#include <math.h>
#include <stddef.h>

/**
 * SECTION: coarray.h
 * @short_description: Coordinate array
 * @title: CoArray
 *
 * Array of cross section coordinates
 *
 * A cross section coordinate is a point in y (lateral) and z (vertical) space.
 */

/**
 * CoArray:
 *
 * Coordinate array used with xs_new() for the creation of #CrossSection.
 */
typedef struct CoArray *CoArray;

/**
 * coarray_new:
 * @n: the length of @y and @z
 * @y: pointer to an array of @n y-values
 * @z: pointer to an array of @n z-values
 *
 * Creates a new coordinate array with length @n and y- and z-values of @y and
 * @z. The resulting coordinate array is newly allocated and must be freed
 * with coarray_free().
 *
 * **Raises**:
 *
 * #null_ptr_arg_Error if @y or @z is `NULL`
 *
 * #coarray_n_coords_Error if @n is less than 2
 *
 * #coarray_y_order_Error if the values in @y are not in acending (or equal)
 * order
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
 * **Raises:**
 *
 * #null_ptr_arg_Error if @ca is `NULL`
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
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a1 or @a2 are `NULL`
 *
 * Returns: 1 if a1 and a2 are equal, 0 if they are not
 */
extern int coarray_eq(CoArray a1, CoArray a2);

/**
 * coarray_length:
 * @a: a #CoArray
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: the length of @a
 */
extern int coarray_length(CoArray a);

/**
 * coarray_get_y:
 * @a: a #CoArray
 * @i: index
 *
 * Returns y-value of the `i`-th coordinate of an array.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * #index_Error if `i < 0` or `i >= length`, where `length` is the length of @a
 *
 * Returns: y-value of `i`-th coordinate
 */
extern double coarray_get_y(CoArray a, int i);

/**
 * coarray_get_z:
 * @a: a #CoArray
 * @i: index
 *
 * Returns z-value of the `i`-th coordinate of an array.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * #index_Error if `i < 0` or `i >= length`, where `length` is the length of @a
 *
 * Returns: z-value of `i`-th coordinate
 */
extern double coarray_get_z(CoArray a, int i);

extern CoArray coarray_subarray_y(CoArray a, double ylo, double yhi);
extern CoArray coarray_subarray_z(CoArray a, double z);

/**
 * coarray_min_z:
 * @a: a #CoArray
 *
 * Returns: the minimum z value in @a.
 */
double coarray_min_z(CoArray a);

/**
 * coarray_add_z:
 * @ca:    a #CoArray
 * @add_z: z-value to add to @ca
 *
 * Translates @ca in the z-direction by @add_z. The resulting coordinate array
 * is newly allocated and should be freed using coarray_free() when no longer
 * needed.
 *
 * Returns: a new copy of @ca translated in the z-direction by @add_z
 */
extern CoArray coarray_add_z(CoArray ca, double add_z);

#endif
