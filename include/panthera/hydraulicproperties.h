#ifndef HYDRAULICPROPS_INCLUDED
#define HYDRAULICPROPS_INCLUDED

#include <panthera/exceptions.h>

/**
 * SECTION: hydraulicproperties.h
 * @short_description: Hydraulic properties
 * @title: HydraulitProps
 *
 * Hydraulic property data
 *
 */

/**
 * hyd_prop:
 * @HP_DEPTH:            Depth of cross section these properties are
 *                          computed at
 * @HP_AREA:             Cross section area
 * @HP_TOP_WIDTH:        Top width
 * @HP_WETTED_PERIMETER: Wetted perimeter
 * @HP_HYDRAULIC_DEPTH:  Hydraulic depth
 * @HP_HYDRAULIC_RADIUS: Hydraulic radius
 * @N_HP:                Number of hydraulic properties
 */
typedef enum {
    HP_DEPTH,
    HP_AREA,
    HP_TOP_WIDTH,
    HP_WETTED_PERIMETER,
    HP_HYDRAULIC_DEPTH,
    HP_HYDRAULIC_RADIUS,
    N_HP
} hyd_prop;

/**
 * HydraulicProps:
 *
 * Hydraulic properties calculated with a #CrossSection using
 * xs_hydraulic_properties()
 */
typedef struct HydraulicProps *HydraulicProps;

/**
 * hp_new:
 *
 * Creates a new #HydraulicProps
 *
 * Returns: a new #HydraulicProps
 */
extern HydraulicProps hp_new(void);

/**
 * hp_free:
 * @hp: a #HydraulicProps
 *
 * Frees a #HydraulicProps
 *
 * Returns: None
 */
extern void hp_free(HydraulicProps hp);

/**
 * hp_get:
 * @hp:   a #HydraulicProps
 * @prop: a #hyd_prop
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @hp is `NULL`
 *
 * Returns: the value of @prop contained in @hp
 */
extern double hp_get(HydraulicProps hp, hyd_prop prop);

/**
 * hp_set:
 * @hp:    a #HydraulicProps
 * @prop:  a #hyd_prop
 * @value: a value to set
 *
 * Sets the @prop in @hp to @value
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @hp is `NULL`
 *
 * Returns: None
 */
extern void hp_set(HydraulicProps hp, hyd_prop prop, double value);

#endif
