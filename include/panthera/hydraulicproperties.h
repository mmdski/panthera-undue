#ifndef HYDRAULICPROPS_INCLUDED
#define HYDRAULICPROPS_INCLUDED

#include "../panthera.h"

/**
 * hyd_prop:
 * @HP_DEPTH:            Depth of cross section these properties are
 *                          computed at
 * @HP_AREA:             Cross section area
 * @HP_TOP_WIDTH:        Top width
 * @HP_WETTED_PERIMETER: Wetted perimeter
 * @HP_HYDRAULIC_DEPTH:  Hydraulic depth
 * @N_HP:                Number of hydraulic properties
 */
typedef enum {
    HP_DEPTH,
    HP_AREA,
    HP_TOP_WIDTH,
    HP_WETTED_PERIMETER,
    HP_HYDRAULIC_DEPTH,
    N_HP
} hyd_prop;

typedef struct HydraulicProps *HydraulicProps;

/* Allocates a space for a new HydraulicProps */
extern HydraulicProps hp_new(void);

/* Frees a HydraulicProps */
extern void hp_free(HydraulicProps hp);

/* Gets a hyd_prop from a HydraulicProps */
extern double hp_get_property(HydraulicProps hp, hyd_prop prop);

/* Sets a hyd_prop in a HydraulicProps */
extern void hp_set_property(HydraulicProps hp, hyd_prop prop, double value);

#endif
