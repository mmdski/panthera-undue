#ifndef COARRAY_INCLUDED
#define COARRAY_INCLUDED

#include <panthera/coordinate.h>

/**
 * SECTION: coarray.h
 * @short_description: Coordinate array
 * @title: CoArray
 *
 * Array containing #Coordinate structures
 */

/**
 * CoArray:
 *
 * Coordinate array
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
 * Returns: a new coordinate array
 */
extern CoArray
coarray_new(int n, double *y, double *z);

/**
 * coarray_copy:
 * @a: a #CoArray
 *
 * Returns a new copy of @a. The returned coordinate array is newly allocated
 * and should be freed using coarray_free().
 *
 * Returns: a copy of @a
 */
extern CoArray
coarray_copy(CoArray a);

/**
 * coarray_free:
 * @a: a #CoArray
 *
 * Frees a coordinate array.
 *
 * Returns: nothing
 */
extern void
coarray_free(CoArray a);

/**
 * coarray_eq:
 * @a1: a #CoArray
 * @a2: another #CoArray
 *
 * Returns: 0 if @a1 and @a2 are equal
 */
extern int
coarray_eq(CoArray a1, CoArray a2);

/**
 * coarray_max_y:
 * @a: a #CoArray
 *
 * Returns: the maximum y value in @a
 */
extern double
coarray_max_y(CoArray a);

/**
 * coarray_min_y:
 * @a: a #CoArray
 *
 * Returns: the minimum y value in @a
 */
extern double
coarray_min_y(CoArray a);

/**
 * coarray_length:
 * @a: a #CoArray
 *
 * Returns: the length of @a
 */
extern int
coarray_length(CoArray a);

/**
 * coarray_get:
 * @a: a #CoArray
 * @i: index
 *
 * Returns a copy of the @i-th coordinate of an array. The returned coordinate
 * is newly created and must be freed with coord_free().
 *
 * Returns: a copy of @i-th coordinate
 */
extern Coordinate
coarray_get(CoArray a, int i);

/**
 * coarray_subarray:
 * @a:   a #CoArray
 * @zlo: low z-value of coordinate range
 * @zhi: high z-value of coordinate range
 *
 * Returns a subset of the coordinates in @a as a new coordinate array. The
 * subset is selected so that the z-values of the coordinates are between @zlo
 * and @zhi. @zlo and @zhi must be within the range of the z values of the
 * coordinates contained in @a, inclusive. The resulting coordinate array is
 * newly created and should be freed with coarray_free() when no longer needed.
 *
 * Returns: a subset of @a
 */
extern CoArray
coarray_subarray(CoArray a, double zlo, double zhi);

/**
 * coarray_subarray_y:
 * @a:   a #CoArray
 * @yhi: high y-value of coordinate range
 *
 * Returns a subset of the coordinates in @a as a new #CoArray. The subset is
 * selected so that the y-values of the coordinates are less than @yhi.
 * Interpolated coordinates are added to the ends of the subarray if @yhi
 * doesn't exactly define the y-values of the first and last coordinates in
 * @a. The resulting #CoArray is newly created and should be freed with
 * coarray_free() when no longer needed.
 *
 * Returns: a subset of @a
 */
extern CoArray
coarray_subarray_y(CoArray a, double y);

#endif
