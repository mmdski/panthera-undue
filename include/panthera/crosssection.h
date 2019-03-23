#ifndef CROSS_SECTION_INCLUDED
#define CROSS_SECTION_INCLUDED

#include "panthera/coarray.h"
#include "panthera/hydraulicproperties.h"

typedef struct CrossSection *CrossSection;

/* Allocates memory and for and initalizes a new CrossSection instance.
 * y_roughness contains y-locations of roughness splits. n_roughness - 1
 * y-values are read from y_roughness and used as locations of the roughness
 * splits.
 */
extern CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                           double *y_roughness);

/* Frees the memory used by a CrossSection */
extern void xs_free(CrossSection xs);

/* Returns a copy of the CoArray contained in xs */
CoArray xs_coarray(CrossSection xs);

/* Returns the hydraulic properties calculated by xs at elevation wse */
extern HydraulicProps xs_hydraulic_properties(CrossSection xs, double wse);

#endif
