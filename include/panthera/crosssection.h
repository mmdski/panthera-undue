#ifndef CROSS_SECTION_INCLUDED
#define CROSS_SECTION_INCLUDED

#include <panthera/coarray.h>
#include <panthera/hydraulicproperties.h>

/**
 * SECTION: crosssection.h
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
 * @y_roughness: array of y-locations of roughness section
 *
 * Creates a new #CrossSection consisting of
 * * the coordinates in @ca,
 * * @n_roughness subsections,
 * * @roughness[`i`] for the `i`-th subsection,
 * * and lateral subsection boundaries in @y_roughness.
 *
 * A new copy of @ca is made, so @ca should be freed using coarray_free() after
 * use. If @n_roughness is 1, @y_roughness is ignored and may be `NULL`.
 *
 * *Subsections*
 *
 * The number of subsections created is equal to @n_roughness. The lateral
 * subsection boundaries are described by the first and last coordinates in @ca
 * and the values in @y_roughness.
 * * For a cross section with one subsection, the subsection is defined by the
 * first and last coordinates in @ca.
 * * For a cross section with more than one subsection, the lateral boundaries
 * of the first subsection are coarray_get_y()(@ca, `0`) and @y_roughness[`0`].
 * The lateral boundaries of the `i`-th subsection are @y_roughness[`i`] and
 * @y_roughness[`i + 1`]. The last subsection is bounded by
 * @y_roughness[@n_roughness `- 2`] and coarray_get_y()(@ca, `length - 1`),
 * where `length` is the length of @ca.
 *
 * **Raises:**
 *
 * #value_arg_Error if
 *   * @n_roughness is less than or equal to 0 or
 *   * any value in @roughness is less than or equal to 0.
 *
 * #null_ptr_arg_Error if
 *   * @roughness is `NULL`
 *   * @n_roughness is greater than 1 and @y_roughness  is `NULL`
 *
 * Returns: a new #CrossSection
 */
extern CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                           double *y_roughness);

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
