#include <glib.h>
#include <math.h>
#include <stddef.h>

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
  XSCoordinate *c = g_new (XSCoordinate, 1);

  c->station = station;
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
  int length;                /* number of coordinates in this array */
  double max_y;              /* maximum y in coarray */
  double min_y;              /* minimum y in coarray */
  XSCoordinate *coordinates; /* array of coordinates */
};

static void
check_z_coordinates (int n,
                     XSCoordinate *coordinates,
                     const char *file,
                     int line)
{
  assert (coordinates);

  for (int i = 1; i < n; i++)
    {
      assert ((*(coordinates + i - 1))->z <= (*(coordinates + i))->z);
    }
}

ChlXSCoords
coarray_new (int n, double *y, double *z)
{
  assert (y);
  assert (z);

  double max_y = -INFINITY;
  double min_y = INFINITY;
  ChlXSCoords a;
  NEW (a);

  a->length = n;
  a->coordinates = mem_calloc (n, sizeof (XSCoordinate), __FILE__, __LINE__);

  int i;
  for (i = 0; i < n; i++)
    {
      *(a->coordinates + i) = coord_new (*(y + i), *(z + i));
      if (*(y + i) > max_y)
        max_y = *(y + i);
      if (*(y + i) < min_y)
        min_y = *(y + i);
    }
  a->max_y = max_y;
  a->min_y = min_y;

  check_z_coordinates (n, a->coordinates, __FILE__, __LINE__);

  return a;
}

static ChlXSCoords
coarray_from_array (int n, XSCoordinate *array)
{
  assert (array);

  ChlXSCoords a;
  XSCoordinate *coordinates;
  double max_y = -INFINITY;
  double min_y = INFINITY;

  XSCoordinate c;

  if (n > 0)
    {
      coordinates = mem_calloc (n, sizeof (XSCoordinate), __FILE__, __LINE__);
      for (int i = 0; i < n; i++)
        {
          c = *(array + i);
          *(coordinates + i) = c;
          if (c)
            {
              if (c->y > max_y)
                max_y = c->y;
              if (c->y < min_y)
                min_y = c->y;
            }
        }
    }
  else
    coordinates = NULL;

  NEW (a);
  a->coordinates = coordinates;
  a->length = n;
  a->max_y = max_y;
  a->min_y = min_y;

  return a;
}

ChlXSCoords
coarray_copy (ChlXSCoords ca)
{
  assert (ca);

  int n = ca->length;

  XSCoordinate *coordinates =
      mem_calloc (n, sizeof (XSCoordinate), __FILE__, __LINE__);
  ChlXSCoords copy;

  for (int i = 0; i < n; i++)
    coordinates[i] = coord_copy (ca->coordinates[i]);

  copy = coarray_from_array (n, coordinates);
  mem_free (coordinates, __FILE__, __LINE__);

  return copy;
}

ChlXSCoords
coarray_add_y (ChlXSCoords ca, double add_y)
{
  assert (ca);

  int n = ca->length;
  double *y = mem_calloc (n, sizeof (double), __FILE__, __LINE__);
  double *z = mem_calloc (n, sizeof (double), __FILE__, __LINE__);

  int i;
  for (i = 0; i < n; i++)
    {
      y[i] = (*(ca->coordinates + i))->y + add_y;
      z[i] = (*(ca->coordinates + i))->z;
    }

  ChlXSCoords new_a = coarray_new (n, y, z);

  mem_free (y, __FILE__, __LINE__);
  mem_free (z, __FILE__, __LINE__);

  return new_a;
}

void
coarray_free (ChlXSCoords a)
{
  assert (a);

  int i;
  XSCoordinate c;
  for (i = 0; i < a->length; i++)
    {
      c = *(a->coordinates + i);
      coord_free (c);
    }

  mem_free ((void *) a->coordinates, __FILE__, __LINE__);

  FREE (a);
}

int
coarray_eq (ChlXSCoords a1, ChlXSCoords a2)
{

  XSCoordinate c1;
  XSCoordinate c2;

  int i;

  if (a1 == a2)
    return 0;

  /* check for either NULL */
  if (!a1 || !a2)
    return 1;

  if (a1->length != a2->length)
    return 1;

  for (i = 0; i < a1->length; i++)
    {
      c1 = *(a1->coordinates + i);
      c2 = *(a2->coordinates + i);
      if (coord_eq (c1, c2) != 0)
        return 1;
    }

  return 0;
}

double
coarray_max_y (ChlXSCoords a)
{
  assert (a);
  return a->max_y;
}

double
coarray_min_y (ChlXSCoords a)
{
  assert (a);
  return a->min_y;
}

int
coarray_length (ChlXSCoords a)
{
  assert (a);
  return a->length;
}

XSCoordinate
coarray_get (ChlXSCoords a, int i)
{
  assert (a);
  assert (0 <= i && i < a->length);

  if (a->coordinates[i])
    return coord_copy (a->coordinates[i]);
  else
    return NULL;
}

/* find the index of the coordinate with the greatest z value that's less than
 * or equal to zlo */
static int
find_zlo_idx (ChlXSCoords a, int lo, int hi, double zlo)
{
  if (lo == hi)
    {
      while (lo > 0 && a->coordinates[lo - 1]->z >= zlo)
        {
          lo--;
        }
      return a->coordinates[lo]->z <= zlo ? lo : -1;
    }

  int mid = (hi + lo) / 2;

  if (zlo < a->coordinates[mid]->z)
    return find_zlo_idx (a, lo, mid, zlo);

  int ret = find_zlo_idx (a, mid + 1, hi, zlo);

  return ret == -1 ? mid : ret;
}

static int
find_zhi_idx (ChlXSCoords a, int n, int lo, int hi, double zhi)
{
  if (lo == hi)
    {
      while (hi < n - 1 && a->coordinates[hi + 1]->z <= zhi)
        {
          hi++;
        }
      return a->coordinates[hi]->z >= zhi ? hi : -1;
    }

  int mid = (hi + lo) / 2;

  if (zhi <= a->coordinates[mid]->z)
    return find_zhi_idx (a, n, lo, mid, zhi);

  int ret = find_zhi_idx (a, n, mid + 1, hi, zhi);

  return ret == -1 ? mid : ret;
}

ChlXSCoords
coarray_subarray_y (ChlXSCoords a, double y)
{
  assert (a);

  int n = a->length;

  /* subarray to return */
  int sa_length;
  List list = list_new ();
  ChlXSCoords sa;
  XSCoordinate *coordinates;

  /* loop variables */
  XSCoordinate c1 = NULL;
  XSCoordinate c2 = NULL;
  XSCoordinate c_last = NULL; /* keep track of the last coordinate added */

  /* check the first coordinate */
  c1 = *(a->coordinates);

  /* if the y of the coordinate is less than or equal to y, add the
   * coordinate to the list
   */
  if (c1->y <= y)
    {
      c_last = coord_copy (c1);
      list_append (list, c_last);
    }

  for (int i = 1; i < n; i++)
    {

      c1 = *(a->coordinates + i - 1);
      c2 = *(a->coordinates + i);

      /* add an interpolated coordinate if coordinates change from
       * above to below or below to above the y value
       */
      if ((c1->y < y && y < c2->y) || (y < c1->y && c2->y < y))
        {
          c_last = coord_interp_z (c1, c2, y);
          list_append (list, c_last);
        }

      /* add c2 if c2.z is at or below z */
      if (c2->y <= y)
        {
          c_last = coord_copy (c2);
          list_append (list, c_last);
        }

      /* if the last coordinate added wasn't NULL,
       * c2 isn't the last coordinate in the array,
       * and c2 is above y,
       * add a NULL spot in the
       */
      if (c_last != NULL && (i < n - 1) && (c2->y > y))
        {
          c_last = NULL;
          list_append (list, c_last);
        }
    }

  sa_length = list_length (list);

  /* don't include the last coordinate if it was null */
  if (c_last == NULL)
    sa_length--;

  coordinates = (XSCoordinate *) list_to_array (list);
  list_free (list);
  sa = coarray_from_array (sa_length, coordinates);
  mem_free (coordinates, __FILE__, __LINE__);

  return sa;
}

ChlXSCoords
coarray_subarray (ChlXSCoords a, double zlo, double zhi)
{
  assert (a);
  assert (zhi > zlo);
  assert (a->coordinates[0]->z <= zlo);
  assert (zhi <= a->coordinates[a->length - 1]->z);

  double eps = 1e-10;
  ChlXSCoords sa;
  XSCoordinate c0;
  XSCoordinate c1;
  XSCoordinate *array =
      mem_calloc (a->length, sizeof (XSCoordinate), __FILE__, __LINE__);

  /* loop variables */
  int i = find_zlo_idx (a, 0, a->length, zlo);
  int j = 0;
  int hi = find_zhi_idx (a, a->length, 0, a->length, zhi);

  c0 = a->coordinates[i];
  c1 = a->coordinates[i + 1];

  if (fabs (c1->z - c0->z) <= eps)
    array[j++] = coord_copy (c0);
  else
    array[j++] = coord_interp_y (c0, c1, zlo);

  while (++i < hi)
    {
      array[j++] = coord_copy (a->coordinates[i]);
    }

  c0 = a->coordinates[i - 1];
  c1 = a->coordinates[i];
  if (fabs (c1->z - c0->z) <= eps)
    array[j++] = coord_copy (c1);
  else
    array[j++] = coord_interp_y (c0, c1, zhi);

  sa = coarray_from_array (j, array);
  mem_free (array, __FILE__, __LINE__);

  return sa;
}
