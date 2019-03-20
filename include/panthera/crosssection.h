#ifndef CROSSSECTION_INCLUDED
#define CROSSSECTION_INCLUDED

typedef struct CrossSection *CrossSection;

/* Allocates memory and for and initalizes a new CrossSection instance.
 * y_roughness contains y-locations of roughness splits. n_roughness - 1
 * y-values are read from y_roughness and used as locations of the roughness
 * splits.
 */
extern CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                           double *y_roughness);

extern void xs_free(CrossSection xs);

#endif
