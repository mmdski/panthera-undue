#ifndef CHLXS_INCLUDED
#define CHLXS_INCLUDED

#include <glib.h>

/**
 * SECTION: ChlXSCoords
 * @short_description: Cross section coordinates
 * @title: ChlXSCoords
 *
 * Array containing #Coordinate structures
 */

/**
 * ChlXSCoords:
 *
 * Coordinate array
 */
typedef struct ChlXSCoords *ChlXSCoords;

/**
 * chl_xscoords_new:
 * @n: the length of @station and @elevation
 * @station: pointer to an array of @n station values
 * @elevation: pointer to an array of @n elevation
 *
 * Creates a new coordinate array with length @n and station and elevation
 * values of @station and @elevation. The resulting coordinate array is newly
 * allocated and must be freed with chl_xscoords_free().
 *
 * Returns: a new array of coordinates
 */
extern ChlXSCoords
chl_xscoords_new (int n, double *station, double *elevation, GError **error);

#endif
