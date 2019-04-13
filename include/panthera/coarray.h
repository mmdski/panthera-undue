#ifndef COARRAY_INCLUDED
#define COARRAY_INCLUDED

/**
 * SECTION: coarray.h
 * @short_description: Coordinate array
 * @title: CoArray
 *
 * Array of cross section coordinates
 *
 * A cross section coordinate is a point in y (vertical) and z (lateral) space.
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
 * #coarray_z_order_Error if the values in @z are not in acending (or equal)
 * order
 *
 * Returns: a new #CoArray
 */
extern CoArray coarray_new(int n, double *y, double *z);

/**
 * coarray_copy:
 * @a: a #CoArray
 *
 * Returns a new copy of @a. The returned #CoArray is newly allocated and
 * should be freed using coarray_free().
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: a copy of @a
 */
extern CoArray coarray_copy(CoArray a);

/**
 * coarray_free:
 * @a: a #CoArray
 *
 * Frees a coordinate array.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: None
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
 * Returns: 1 if @a1 and @a2 are equal, 0 if they are not
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
 * Returns y-value of the @i-th coordinate of an array.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * #index_Error if `i < 0` or `i >= length`, where `length` is the length of
 * @a
 *
 * Returns: y-value of @i-th coordinate
 */
extern double coarray_get_y(CoArray a, int i);

/**
 * coarray_get_z:
 * @a: a #CoArray
 * @i: index
 *
 * Returns z-value of the @i-th coordinate of an array.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * #index_Error if `i < 0` or `i >= length`, where `length` is the length of @a
 *
 * Returns: z-value of @i-th coordinate
 */
extern double coarray_get_z(CoArray a, int i);

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
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: a subset of @a
 */
extern CoArray coarray_subarray_y(CoArray a, double yhi);

/**
 * coarray_subarray_z:
 * @a:   a #CoArray
 * @zlo: low z-value of coordinate range
 * @zhi: high z-value of coordinate range
 *
 * Returns a subset of the coordinates in @a as a new #CoArray. The subset is
 * selected so that the z-values of the coordinates are between @zlo and @zhi.
 * Interpolated coordinates are added to the ends of the subarray if @zlo and
 * @zhi don't exactly define the z-values of the first and last coordinates in
 * @a. The resulting #CoArray is newly created and should be freed with
 * coarray_free() when no longer needed.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * #value_arg_Error if zhi <= zlo
 *
 * Returns: a subset of @a
 */
extern CoArray coarray_subarray_z(CoArray a, double zlo, double zhi);

/**
 * coarray_min_y:
 * @a: a #CoArray
 *
 * **Raises**:
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: the minimum y value in @a
 */
double coarray_min_y(CoArray a);

/**
 * coarray_add_y:
 * @a:     a #CoArray
 * @add_y: y-value to add to @a
 *
 * Translates @a in the y-direction by @add_y. The returned #CoArray is newly
 * allocated and should be freed using coarray_free() when no longer needed.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @a is `NULL`
 *
 * Returns: @a translated in the y-direction by @add_y
 */
extern CoArray coarray_add_y(CoArray a, double add_y);

#endif
