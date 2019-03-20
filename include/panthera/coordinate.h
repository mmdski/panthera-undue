#ifndef XS_COORDINATE_INCLUDED
#define XS_COORDINATE_INCLUDED

#include "panthera.h"

/* Except_T raised when coordinate interpolation */
extern const Except_T coord_interp_Fail;

typedef struct Coordinate *Coordinate;

/* Creates and allocate space for a new Coordinate */
Coordinate coord_new(double y, double z);

/* Frees space from a previously allocated Coordinate */
void coord_free(Coordinate c);

/* Makes a copy and returns a new Coordinate */
Coordinate coord_copy(Coordinate c);

/* Returns 1 if c1 and c2 are equal, 0 otherwise */
int coord_eq(Coordinate c1, Coordinate c2);

/* x value from Coordinate */
double coord_y(Coordinate c);

/* y value from Coordinate */
double coord_z(Coordinate c);

/* Linearly interpolates Coordinate given a y value */
Coordinate coord_interp_y(Coordinate c1, Coordinate c2, double y);

/* Linearly interpolates Coordinate given a z value */
Coordinate coord_interp_z(Coordinate c1, Coordinate c2, double z);

#endif
