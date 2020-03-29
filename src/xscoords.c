#include "mem.h"
#include <chulengo/error.h>
#include <chulengo/xs.h>
#include <math.h>

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
XSCoordinate *
xscoord_new (double station, double elevation)
{
  XSCoordinate *c = mem_alloc (sizeof (XSCoordinate));

  c->station   = station;
  c->elevation = elevation;

  return c;
}

/**
 * xscoord_copy:
 * @c: a #XSCoordinate
 *
 * Creates a new copy of @c. The returned coordinate is newly created and
 * should be freed using coord_free().
 *
 * Returns: a copy of @c
 */
XSCoordinate *
xscoord_copy (XSCoordinate *c)
{
  g_assert (c);
  return xscoord_new ((c->station), (c->elevation));
}

/**
 * xscoord_free:
 * @c: a #XSCoordinate
 *
 * Frees a coordinate
 *
 * Returns: nothing
 */
void
xscoord_free (XSCoordinate *c)
{
  g_free (c);
}

/**
 * xscoord_eq:
 * @c1: a #XSCoordinate
 * @c2: another #XSCoordinate
 *
 * Returns: 0 if @c1 and @c2 are equal
 */
int
xscoord_eq (XSCoordinate *c1, XSCoordinate *c2)
{
  if (c1 == c2)
    return 0;
  /* either coordinate is NULL */
  if (!c1 || !c2)
    return 1;

  if (c1->station == c2->station && c1->elevation == c2->elevation)
    return 0;
  else
    return 1;
}

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
XSCoordinate *
xscoord_interpelevation (XSCoordinate *c1, XSCoordinate *c2, double station)
{
  g_assert (c1 && c2);

  /* assert station is between the two points */
  g_assert ((c1->station <= station && station <= c2->station) ||
            (c2->station <= station && station <= c1->station));

  double slope = (c2->elevation - c1->elevation) / (c2->station - c1->station);
  double elevation = slope * (station - c1->station) + c1->elevation;
  return xscoord_new (station, elevation);
}

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
XSCoordinate *
xscoord_interpstation (XSCoordinate *c1, XSCoordinate *c2, double elevation)
{
  g_assert (c1 && c2);

  /* assert z is between the two points */
  g_assert ((c1->elevation <= elevation && elevation <= c2->elevation) ||
            (c2->elevation <= elevation && elevation <= c1->elevation));

  double slope = (c2->station - c1->station) / (c2->elevation - c1->elevation);
  double station = slope * (elevation - c1->elevation) + c1->station;
  return xscoord_new (station, elevation);
}

struct ChlXSCoords
{
  int            length;        /* number of coordinates in this array */
  double         max_elevation; /* maximum elevation in coarray */
  double         min_elevation; /* minimum elevation in coarray */
  XSCoordinate **coordinates;   /* array of coordinates */
};

ChlXSCoords
chl_xscoords_new (int n, double *station, double *elevation, GError **error)
{

  g_return_val_if_fail (station == NULL || elevation == NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (n < 2)
    {
      g_set_error (error,
                   CHL_ERROR,
                   CHL_ERROR_ARG,
                   "n must be greater than or equal to 2");
      return NULL;
    }

  // check that station points are in ascending order before allocating any
  // memory
  int i;
  for (i = 1; i < n; i++)
    {
      if (*(station + i - 1) > *(station + i))
        {
          g_set_error (error,
                       CHL_ERROR,
                       CHL_ERROR_ARG,
                       "station must be in ascending order");
          return NULL;
        }
    }

  double max_y = -INFINITY;
  double min_y = INFINITY;

  ChlXSCoords a;
  NEW (a);

  XSCoordinate **coords = mem_calloc (sizeof (XSCoordinate *), n);

  for (i = 0; i < n; i++)
    {
      *(coords + i) = xscoord_new (*(station + i), *(elevation + i));

      if (*(station + i) > max_y)
        max_y = *(station + i);
      if (*(station + i) < min_y)
        min_y = *(station + i);
    }

  a->length        = n;
  a->coordinates   = coords;
  a->max_elevation = max_y;
  a->min_elevation = min_y;

  return a;
}
