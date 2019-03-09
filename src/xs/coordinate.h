#ifndef XS_COORDINATE_INCLUDED
#define XS_COORDINATE_INCLUDED

#include "cii/except.h"

#define T Coordinate_T

/* Except_T raised when coordinate interpolation */
extern const Except_T coord_interp_y_Fail;

typedef struct T *T;

/* Creates and allocate space for a new Coordinate_T */
T coord_new(double x, double y);

/* Frees space from a previously allocated Coordinate_T */
void coord_free(T c);

/* x value from Coordinate_T */
double coord_x(T c);

/* y value from Coordinate_T */
double coord_y(T c);

/* Linearly interpolates Coordinate_T given a y value */
T coord_interp_y(T c1, T c2, double y);

#undef T
#endif
