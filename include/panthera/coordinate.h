#ifndef COORDINATE_INCLUDED
#define COORDINATE_INCLUDED

/**
 * SECTION: coordinate.h
 * @short_description: Coordinate
 * @title: Coordinate
 *
 * y, z coordinate
 *
 * A coordinate is a point in y (vertical) and z (lateral) space.
 */

/**
 * Coordinate:
 * @y: vertical value of coordinate
 * @z: lateral value of coordinate
 *
 * y, z coordinate
 */
struct Coordinate {
    double y; /* vertical coordinate */
    double z; /* lateral coordinate */
};

/**
 * Coordinate:
 *
 * y, z coordinate
 */
typedef struct Coordinate *Coordinate;

/**
 * coord_new:
 * @y: vertical value of coordinate
 * @z: lateral value of coordinate
 *
 * Creates a new coordinate with @y and @z as the vertical and lateral values,
 * respectively. The resulting coordinate is newly allocated and should be
 * freed with coord_free().
 *
 * Returns: a new coordinate
 */
extern Coordinate
coord_new(double y, double z);

/**
 * coord_copy:
 * @c: a #Coordinate
 *
 * Creates a new copy of @c. The returned coordinate is newly created and
 * should be freed using coord_free().
 *
 * Returns: a copy of @c
 */
extern Coordinate
coord_copy(Coordinate c);

/**
 * coord_free:
 * @c: a #Coordinate
 *
 * Frees a coordinate
 *
 * Returns: nothing
 */
extern void
coord_free(Coordinate c);

/**
 * coord_eq:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 *
 * Returns: 0 if @c1 and @c2 are equal
 */
extern int
coord_eq(Coordinate c1, Coordinate c2);

/**
 * coord_interp_y:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 * @z: lateral value
 *
 * Creates a new coordinate with a @y that is linearly interpolated using the
 * values of @c1 and @c2 and @z. The new coordinate will have a vertical value
 * that is equal to the interpolated value and a lateral value equal to @z. The
 * returned coordinate is newly created and should be freed using
 * @coord_free().
 *
 * Returns: an interpolated coordinate
 */
extern Coordinate
coord_interp_y(Coordinate c1, Coordinate c2, double z);

/**
 * coord_interp_z:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 * @y: vertical value
 *
 * Creates a new coordinate with a @z that is linearly interpolated using the
 * values of @c1 and @c2 and @y. The new coordinate will have a lateral value
 * that is equal to the interpolated value and a lateral value equal to @y. The
 * returned coordinate is newly created and should be freed using
 * @coord_free().
 *
 * Returns: an interpolated coordinate
 */
extern Coordinate
coord_interp_z(Coordinate c1, Coordinate c2, double y);

#endif
