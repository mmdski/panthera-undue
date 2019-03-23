#ifndef SRC_COARRAY_INCLUDE
#define SRC_COARRAY_INCLUDE

#include "panthera/coarray.h"

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
