#ifndef XS_SS_INCLUDED
#define XS_SS_INCLUDED

#include "coarray.h"
#include "panthera/hydraulicproperties.h"

#define T Subsection

typedef struct T *T;

/* Allocates memory and creates a new Subsection */
extern T ss_new(int n, double *x, double *y, double roughness,
                double activation_depth);

/* Frees memory from a previously allocated T */
extern void ss_free(T ss);

/* Calculates the area in the subsection */
extern double ss_area(T ss, double y);

/* Calculates the perimeter of the subsection */
extern double ss_perimeter(T ss, double y);

/* Calculates the top width of the subsection */
extern double ss_top_width(T ss, double y);

/* Calculates hydraulic properties for the subsection.
 * Returns a new HydraulicProps.
 */
extern HydraulicProps ss_hydraulic_properties(T ss, double y);

#undef T
#endif
