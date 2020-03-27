#include "xscoord.h"
#include <glib.h>

/* Creates and allocate space for a new XSCoordinate */
XSCoordinate *
xscoord_new (double station, double elevation)
{
  XSCoordinate *c = g_new (XSCoordinate, 1);

  c->station = station;
  c->elevation = elevation;

  return c;
}

/* Makes a copy and returns a new Coordinate */
XSCoordinate *
xscoord_copy (XSCoordinate *c)
{
  g_assert (c);
  return xscoord_new ((c->station), (c->elevation));
}

/* Frees space from a previously allocated Coordinate */
void
xscoord_free (XSCoordinate *c)
{
  g_free (c);
}

/* Returns 0 if c1 and c2 are equal */
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

/* Linearly interpolates XSCoordinate elevation value given a station value */
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

/* Linearly interpolates XSCoordinate station value given an elevation value */
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
