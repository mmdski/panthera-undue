#ifndef XS_SS_INCLUDED
#define XS_SS_INCLUDED

#include "panthera.h"

typedef struct Subsection *Subsection;

/* Allocates memory and creates a new Subsection */
extern Subsection ss_new(CoArray ca, double roughness,
                         double activation_depth);

/* Frees memory from a previously allocated Subsection */
extern void ss_free(Subsection ss);

/* Calculates the area in the subsection */
extern double ss_area(Subsection ss, double z);

/* Calculates the perimeter of the subsection */
extern double ss_perimeter(Subsection ss, double z);

/* Calculates the top width of the subsection */
extern double ss_top_width(Subsection ss, double z);

/* Calculates hydraulic properties for the subsection.
 * Returns a new HydraulicProps.
 */
extern HydraulicProps ss_hydraulic_properties(Subsection ss, double z);

#endif
