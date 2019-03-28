#ifndef CROSS_SECTION_INCLUDED
#define CROSS_SECTION_INCLUDED

#include <panthera/coarray.h>

/**
 * SECTION: Hydraulic Properties
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
 * @HP_CONVEYANCE:       Conveyance
 * @N_HP:                Number of hydraulic properties
 */
typedef enum {
    HP_DEPTH,
    HP_AREA,
    HP_TOP_WIDTH,
    HP_WETTED_PERIMETER,
    HP_HYDRAULIC_DEPTH,
    HP_HYDRAULIC_RADIUS,
    HP_CONVEYANCE,
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
 * SECTION: Cross Section
 * @short_description: Cross section
 * @title: CrossSection
 *
 * Hydraulic cross section
 */

/**
 * CrossSection:
 *
 * The cross section interface calculates hydraulic parameters using geometry
 * defined by a #CoArray.
 */
typedef struct CrossSection *CrossSection;

/**
 * xs_new:
 * @ca:          a #CoArray
 * @n_roughness: number of roughness values in cross section
 * @roughness:   array of @n_roughness values
 * @z_roughness: array of z-locations of roughness section
 *
 * Creates a new #CrossSection consisting of
 * * the coordinates in @ca,
 * * @n_roughness subsections,
 * * @roughness[`i`] for the `i`-th subsection,
 * * and lateral subsection boundaries in @z_roughness.
 *
 * A new copy of @ca is made, so @ca should be freed using coarray_free() after
 * use. If @n_roughness is 1, @z_roughness is ignored and may be `NULL`.
 * Otherwise, the length of @z_roughness is @n_roughness `- 1`.
 *
 * *Subsections*
 *
 * The number of subsections created is equal to @n_roughness. The lateral
 * subsection boundaries are described by the first and last coordinates in @ca
 * and the values in @z_roughness.
 * * For a cross section with one subsection, the subsection is defined by the
 * first and last coordinates in @ca.
 * * For a cross section with more than one subsection, the lateral boundaries
 * of the first subsection are coarray_get_z()( @ca, `0`) and
 * @z_roughness[`0`]. The lateral boundaries of the `i`-th subsection are
 * @z_roughness[`i`] and @z_roughness[`i + 1`]. The last subsection is bounded
 * by @z_roughness[ @n_roughness `- 2`] and
 * coarray_get_z()( @ca, `length - 1`), where `length` is the length of @ca.
 *
 * **Raises:**
 *
 * #value_arg_Error if
 *   * @n_roughness is less than or equal to 0 or
 *   * any value in @roughness is less than or equal to 0.
 *
 * #null_ptr_arg_Error if
 *   * @roughness is `NULL`
 *   * @n_roughness is greater than 1 and @z_roughness  is `NULL`
 *
 * Returns: a new #CrossSection
 */
extern CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                           double *z_roughness);

/**
 * xs_free:
 * @xs: a #CrossSection
 *
 * Frees a #CrossSection.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @xs is `NULL`
 *
 * Returns: None
 */
extern void xs_free(CrossSection xs);

/**
 * xs_coarray:
 * @xs: a #CrossSection
 *
 * The resulting coordinate array is newly created and should be freed with
 * coarray_free() after use.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @xs is `NULL`
 *
 * Returns: a copy of the #CoArray contained in @xs
 */
CoArray xs_coarray(CrossSection xs);

/**
 * xs_hydraulic_properties:
 * @xs:  a #CrossSection
 * @wse: water surface elevation
 *
 * The returned #HydraulicProps is newly created and should be freed with
 * hp_free() after use.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @xs is `NULL`
 *
 * Returns: the hydraulic properties calculated by @xs at elevation @wse
 */
extern HydraulicProps xs_hydraulic_properties(CrossSection xs, double wse);

#endif
