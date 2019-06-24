#ifndef CROSSSECTION_INCLUDED
#define CROSSSECTION_INCLUDED

#include <panthera/coarray.h>

/**
 * SECTION: Cross Section
 * @short_description: Cross section
 * @title: CrossSection
 *
 * Hydraulic cross section
 */

/**
 * xs_prop:
 * @XS_DEPTH:             Cross section depth of computed properties
 * @XS_AREA:              Cross section area
 * @XS_TOP_WIDTH:         Top width
 * @XS_WETTED_PERIMETER:  Wetted perimeter
 * @XS_HYDRAULIC_DEPTH:   Hydraulic depth
 * @XS_HYDRAULIC_RADIUS:  Hydraulic radius
 * @XS_CONVEYANCE:        Conveyance
 * @XS_VELOCITY_COEFF:    Velocity coefficient
 * @XS_CRITICAL_FLOW:     Critical flow
 * @N_XSP:                Number of hydraulic properties
 */
typedef enum {
    XS_DEPTH,
    XS_AREA,
    XS_TOP_WIDTH,
    XS_WETTED_PERIMETER,
    XS_HYDRAULIC_DEPTH,
    XS_HYDRAULIC_RADIUS,
    XS_CONVEYANCE,
    XS_VELOCITY_COEFF,
    XS_CRITICAL_FLOW,
    N_XSP
} xs_prop;

/**
 * CrossSectionProps:
 *
 * Hydraulic properties calculated with a #CrossSection using
 * xs_hydraulic_properties()
 */
typedef struct CrossSectionProps *CrossSectionProps;

/**
 * xsp_free:
 * @xsp: a #CrossSectionProps
 *
 * Frees a #CrossSectionProps
 *
 * Returns: None
 */
extern void
xsp_free (CrossSectionProps xsp);

/**
 * xsp_get:
 * @xsp:   a #CrossSectionProps
 * @prop:  a #xs_prop
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xsp is `NULL`
 *
 * Returns: the value of @prop contained in @xsp
 */
extern double
xsp_get (CrossSectionProps xsp, xs_prop prop);

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
 * #value_arg_error if
 *   * @n_roughness is less than or equal to 0 or
 *   * any value in @roughness is less than or equal to 0.
 *
 * #null_ptr_arg_error if
 *   * @roughness is `NULL`
 *   * @n_roughness is greater than 1 and @z_roughness  is `NULL`
 *
 * Returns: a new #CrossSection
 */
extern CrossSection
xs_new (CoArray ca, int n_roughness, double *roughness, double *z_roughness);

/**
 * xs_free:
 * @xs: a #CrossSection
 *
 * Decreases the reference count of @xs. Frees @xs if the reference count is
 * zero.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs is `NULL`
 *
 * Returns: None
 */
extern void
xs_free (CrossSection xs);

/**
 * xs_coarray:
 * @xs: a #CrossSection
 *
 * The resulting coordinate array is newly created and should be freed with
 * coarray_free() after use.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs is `NULL`
 *
 * Returns: a copy of the #CoArray contained in @xs
 */
CoArray
xs_coarray (CrossSection xs);

/**
 * xs_n_subsections:
 * @xs: a #CrossSection
 *
 * Returns the number of sections in @xs.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs is `NULL`
 *
 * Returns: number of subsections
 */
extern int
xs_n_subsections (CrossSection xs);

/**
 * xs_roughness:
 * @xs: a #CrossSection
 * @roughness: a pointer to an array of doubles
 *
 * Fills @roughness with the roughness values of the subsections in @xs.
 *
 * @roughness must be allocated as an array of doubles with the number of
 * subsections in @xs as the number of elements. Use xs_n_subsections() to get
 * the number of subsections in @xs.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs or @roughness are `NULL`
 *
 * Returns: None
 */
extern void
xs_roughness (CrossSection xs, double *roughness);

/**
 * xs_z_roughness:
 * @xs: a #CrossSection
 * @z_roughness: a pointer to an array of doubles
 *
 * Fills @z_roughness with the z-values of the subsection splits in @xs.
 *
 * @roughness must be allocated as an array of doubles with the number of
 * subsections in @xs minus one as the number of elements. Use
 * xs_n_subsections() to get the number of subsections in @xs.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs or @z_roughness are `NULL`
 *
 * Returns: None
 */
extern void
xs_z_roughness (CrossSection xs, double *z_roughness);

/**
 * xs_hydraulic_properties:
 * @xs:  a #CrossSection
 * @h:   depth
 *
 * The returned #CrossSectionProps is newly created and should be freed with
 * xsp_free() after use.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs is `NULL`
 *
 * #xsp_depth_error if @h is less than the lowest y-value in @xs coordinate
 *
 * #xs_invld_depth_error if @h is Nan or Inf
 *
 * Returns: the hydraulic properties calculated by @xs at depth @h
 */
extern CrossSectionProps
xs_hydraulic_properties (CrossSection xs, double h);

/**
 * xs_critical_depth
 * @xs:            a #CrossSection
 * @critical_flow: critical flow value
 * @initial_depth: initial depth for solution
 *
 * Computes critical depth using secant solver. Returns NAN if no solution is
 * found.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xs is `NULL`
 *
 * #value_arg_error if @critical_flow or @initial_depth are not finite or if
 * @initial_depth is less than the minimum y value in @xs
 *
 * Returns: critical depth computed for @critical_flow
 */
extern double
xs_critical_depth (CrossSection xs,
                   double       critical_flow,
                   double       initial_depth);

#endif
