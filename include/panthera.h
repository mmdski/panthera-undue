#ifndef PANTHERA_INCLUDED
#define PANTHERA_INCLUDED

#include "cii/assert.h"
#include "cii/except.h"
#include "cii/mem.h"
#include <math.h>

/** exception raised when a function is passed a null pointer */
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

/** Allocates a space for a new HydraulicProps */
extern HydraulicProps hp_new(void);

/** Frees a HydraulicProps */
extern void hp_free(HydraulicProps hp);

/** Gets a hyd_prop from a HydraulicProps */
extern double hp_get_property(HydraulicProps hp, hyd_prop prop);

/** Sets a hyd_prop in a HydraulicProps */
extern void hp_set_property(HydraulicProps hp, hyd_prop prop, double value);


/* coordinate array */

/** Exception raised when there are too few coordinates passed to an
 *  initializer
 */
extern const Except_T coarray_n_coords_Error;

/** Exception raised when the y-order of coordinates is incorrect
 */
extern const Except_T coarray_y_order_Error;

/** Coordinate array type definition
 */
typedef struct CoArray *CoArray;

/** Allocates memory and creates a new CoArray
 */
extern CoArray coarray_new(int n, double *y, double *z);

/** Allocates memory and returns a copy of CoArray ca
 */
extern CoArray coarray_copy(CoArray ca);

/** Frees the memory from a previously created CoArray
 */
extern void coarray_free(CoArray a);

/** Returns 1 if a1 and a2 are equal, 0 otherwise
 */
extern int coarray_eq(CoArray a1, CoArray a2);

/** The number of coordinates in CoArray a
 */
extern int coarray_length(CoArray a);

/** Returns the y value of the i-th coordinate, or NAN if the i-th value
 *  is NULL.
 */
extern double coarray_get_y(CoArray a, int i);

/** Returns the z value of the i-th coordinate, or NAN if the i-th value
 *  is NULL.
 */
extern double coarray_get_z(CoArray a, int i);

/** Returns the minimum z value in the CoArray
 */
double coarray_min_z(CoArray a);

/** Returns a subarray between ylo and yhi y values (inclusive)
 */
extern CoArray coarray_subarray_y(CoArray a, double ylo, double yhi);

/** Returns a subarray with points below z value
 */
extern CoArray coarray_subarray_z(CoArray a, double z);

/** Returns a CoArray z-translated by add_z
 */
extern CoArray coarray_add_z(CoArray ca, double add_z);


/* cross section */

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
