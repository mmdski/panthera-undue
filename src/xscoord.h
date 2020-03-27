#ifndef XSCOORD_INCLUDED
#define XSCOORD_INCLUDED

/**
 * SECTION: xscoordinate.h
 * @short_description: Cross section coordinate
 * @title: XSCoordinate
 *
 * Station, elevation coordinates for cross section points
 *
 * A coordinate is a point in station (lateral) and elevation (vertical) space.
 */

/**
 * XSCoordinate:
 * @station: lateral value of coordinate
 * @elevation: vertical value of coordinate
 *
 * station, elevation coordinate
 */
typedef struct XSCoordinate
{
  double station;   /* lateral coordinate */
  double elevation; /* elevation coordinate */
} XSCoordinate;

/**
 * xscoord_new:
 * @station: lateral value of coordinate
 * @elevation: vertical value of coordinate
 *
 * Creates a new coordinate with @station and @elevation as the lateral and
 * vertical values, respectively. The resulting coordinate is newly allocated
 * and should be freed with xscoord_free().
 *
 * Returns: a new coordinate
 */
extern XSCoordinate *xscoord_new (double station, double elevation);

/**
 * xscoord_copy:
 * @c: a #XSCoordinate
 *
 * Creates a new copy of @c. The returned coordinate is newly created and
 * should be freed using coord_free().
 *
 * Returns: a copy of @c
 */
extern XSCoordinate *xscoord_copy (XSCoordinate *c);

/**
 * xscoord_free:
 * @c: a #XSCoordinate
 *
 * Frees a coordinate
 *
 * Returns: nothing
 */
extern void xscoord_free (XSCoordinate *c);

/**
 * xscoord_eq:
 * @c1: a #XSCoordinate
 * @c2: another #XSCoordinate
 *
 * Returns: 0 if @c1 and @c2 are equal
 */
extern int xscoord_eq (XSCoordinate *c1, XSCoordinate *c2);

/**
 * xscoord_interpelevation:
 * @c1: a #XSCoordinate
 * @c2: another #XSCoordinate
 * @station: lateral value
 *
 * Creates a new coordinate with an @elevation that is linearly interpolated
 * using the values of @c1 and @c2 and @elevation. The new coordinate will have
 * a vertical value that is equal to the interpolated value and a lateral value
 * equal to @station. The returned coordinate is newly created and should be
 * freed using @xscoord_free().
 *
 * Returns: an interpolated coordinate
 */
extern XSCoordinate *
xscoord_interpelevation (XSCoordinate *c1, XSCoordinate *c2, double station);

/**
 * xscoord_interpstation:
 * @c1: a #XSCoordinate
 * @c2: another #XSCoordinate
 * @elevation: vertical value
 *
 * Creates a new coordinate with a @station that is linearly interpolated using
 * the values of @c1 and @c2 and @elevation. The new coordinate will have a
 * lateral value that is equal to the interpolated value and a vertical value
 * equal to @elevation. The returned coordinate is newly created and should be
 * freed using @coord_free().
 *
 * Returns: an interpolated coordinate
 */
extern XSCoordinate *
xscoord_interpstation (XSCoordinate *c1, XSCoordinate *c2, double elevation);

#endif