#ifndef CROSSSECTION_INCLUDED
#define CROSSSECTION_INCLUDED

typedef struct CrossSection *CrossSection;

extern CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                           double *y_roughness);

extern void xs_free(CrossSection xs);

#endif
