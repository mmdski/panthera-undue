#ifndef XS_SUBSECTION_INCLUDED
#define XS_SUBSECTION_INCLUDED

#include "coarray.h"

#define T Subsection_T

typedef struct T *T;

/* Allocates memory and creates a new Subsection_T */
extern T subsection_new(int n, double *x, double *y, double roughness,
                        double activation_depth);

/* Frees memory from a previously allocated T */
extern void subsection_free(T ss);

/* Calculates the area in the subsection */
extern double subsection_area(T ss, double y);

/* Calculates the perimeter of the subsection */
extern double subsection_perimeter(T ss, double y);

/* Calculates the top width of the subsection */
extern double subsection_top_width(T ss, double y);

#undef T
#endif
