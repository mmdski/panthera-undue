#ifndef XS_COORDINATE_INCLUDED
#define XS_COORDINATE_INCLUDED

#include "cii/except.h"

#define T Coordinate

/* Except_T raised when coordinate interpolation */
extern const Except_T coord_interp_Fail;

typedef struct T *T;

/* Creates and allocate space for a new Coordinate */
T coord_new(double y, double z);

/* Frees space from a previously allocated Coordinate */
void coord_free(T c);

/* Makes a copy and returns a new Coordinate */
T coord_copy(T c);

/* Returns 1 if c1 and c2 are equal, 0 otherwise */
int coord_eq(T c1, T c2);

/* x value from Coordinate */
double coord_y(T c);

/* y value from Coordinate */
double coord_z(T c);

/* Linearly interpolates Coordinate given a y value */
T coord_interp_y(T c1, T c2, double z);

/* Linearly interpolates Coordinate given a z value */
T coord_interp_z(T c1, T c2, double z);

#undef T
#endif
