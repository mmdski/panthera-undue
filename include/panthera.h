#ifndef PANTHERA_INCLUDED
#define PANTHERA_INCLUDED

#include "cii/except.h"

/* exception raised when a function is passed a null pointer */
extern const Except_T null_ptr_arg_Error;

/* hydraulic properties */
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
