#ifndef HYDRAULIC_PROPERTIES_INCLUDED
#define HYDRAULIC_PROPERTIES_INCLUDED

#define T HydraulicProps

typedef enum {
    HP_DEPTH,
    HP_AREA,
    HP_TOP_WIDTH,
    HP_WETTED_PERIMETER,
    HP_HYDRAULIC_DEPTH,
    N_HP
} hyd_prop;

typedef struct T *T;

/* Allocates a space for a new HydraulicProps */
extern T hp_new(void);

/* Frees a HydraulicProps */
extern void hp_free(T hp);

/* Gets a hyd_prop from a HydraulicProps */
extern double hp_get_property(T hp, hyd_prop prop);

/* Sets a hyd_prop in a HydraulicProps */
extern void hp_set_property(T hp, hyd_prop prop, double value);

#undef T
#endif
