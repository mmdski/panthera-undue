#ifndef XS_SS_INCLUDED
#define XS_SS_INCLUDED

#include "coarray.h"
#include "panthera/hydraulicproperties.h"

#define T Subsection

typedef struct T *T;

/* Allocates memory and creates a new Subsection */
extern T ss_new(CoArray ca, double roughness, double activation_depth);

/* Frees memory from a previously allocated Subsection */
extern void ss_free(T ss);

/* Calculates the area in the subsection */
extern double ss_area(T ss, double z);

/* Calculates the perimeter of the subsection */
extern double ss_perimeter(T ss, double z);

/* Calculates the top width of the subsection */
extern double ss_top_width(T ss, double z);

/* Calculates hydraulic properties for the subsection.
 * Returns a new HydraulicProps.
 */
extern HydraulicProps ss_hydraulic_properties(T ss, double z);

#undef T
#endif
