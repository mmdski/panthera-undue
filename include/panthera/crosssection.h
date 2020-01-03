#ifndef CROSSSECTION_INCLUDED
#define CROSSSECTION_INCLUDED


/**
 * SECTION: coordinate.h
 * @short_description: Coordinate
 * @title: Coordinate
 *
 * y, z coordinate
 *
 * A coordinate is a point in y (vertical) and z (lateral) space.
 */

/**
 * Coordinate:
 * @y: vertical value of coordinate
 * @z: lateral value of coordinate
 *
 * y, z coordinate
 */
struct Coordinate {
    double y; /* vertical coordinate */
    double z; /* lateral coordinate */
};

/**
 * Coordinate:
 *
 * y, z coordinate
 */
typedef struct Coordinate *Coordinate;

/**
 * coord_new:
 * @y: vertical value of coordinate
 * @z: lateral value of coordinate
 *
 * Creates a new coordinate with @y and @z as the vertical and lateral values,
 * respectively. The resulting coordinate is newly allocated and should be
 * freed with coord_free().
 *
 * Returns: a new coordinate
 */
extern Coordinate
coord_new(double y, double z);

/**
 * coord_copy:
 * @c: a #Coordinate
 *
 * Creates a new copy of @c. The returned coordinate is newly created and
 * should be freed using coord_free().
 *
 * Returns: a copy of @c
 */
extern Coordinate
coord_copy(Coordinate c);

/**
 * coord_free:
 * @c: a #Coordinate
 *
 * Frees a coordinate
 *
 * Returns: nothing
 */
extern void
coord_free(Coordinate c);

/**
 * coord_eq:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 *
 * Returns: 0 if @c1 and @c2 are equal
 */
extern int
coord_eq(Coordinate c1, Coordinate c2);

/**
 * coord_interp_y:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 * @z: lateral value
 *
 * Creates a new coordinate with a @y that is linearly interpolated using the
 * values of @c1 and @c2 and @z. The new coordinate will have a vertical value
 * that is equal to the interpolated value and a lateral value equal to @z. The
 * returned coordinate is newly created and should be freed using
 * @coord_free().
 *
 * Returns: an interpolated coordinate
 */
extern Coordinate
coord_interp_y(Coordinate c1, Coordinate c2, double z);

/**
 * coord_interp_z:
 * @c1: a #Coordinate
 * @c2: another #Coordinate
 * @y: vertical value
 *
 * Creates a new coordinate with a @z that is linearly interpolated using the
 * values of @c1 and @c2 and @y. The new coordinate will have a lateral value
 * that is equal to the interpolated value and a lateral value equal to @y. The
 * returned coordinate is newly created and should be freed using
 * @coord_free().
 *
 * Returns: an interpolated coordinate
 */
extern Coordinate
coord_interp_z(Coordinate c1, Coordinate c2, double y);

/**
 * SECTION: coarray.h
 * @short_description: Coordinate array
 * @title: CoArray
 *
 * Array containing #Coordinate structures
 */

/**
 * CoArray:
 *
 * Coordinate array
 */
typedef struct CoArray *CoArray;

/**
 * coarray_new:
 * @n: the length of @y and @z
 * @y: pointer to an array of @n y-values
 * @z: pointer to an array of @n z-values
 *
 * Creates a new coordinate array with length @n and y- and z-values of @y and
 * @z. The resulting coordinate array is newly allocated and must be freed
 * with coarray_free().
 *
 * Returns: a new coordinate array
 */
extern CoArray
coarray_new(int n, double *y, double *z);

/**
 * coarray_copy:
 * @a: a #CoArray
 *
 * Returns a new copy of @a. The returned coordinate array is newly allocated
 * and should be freed using coarray_free().
 *
 * Returns: a copy of @a
 */
extern CoArray
coarray_copy(CoArray a);

/**
 * coarray_free:
 * @a: a #CoArray
 *
 * Frees a coordinate array.
 *
 * Returns: nothing
 */
extern void
coarray_free(CoArray a);

/**
 * coarray_add_y:
 * @a:     a #CoArray
 * @add_y: y-value to add to @a
 *
 * Translates @a in the y-direction by @add_y. The returned coordinate array is
 * newly allocated and should be freed using coarray_free() when no longer
 * needed.
 *
 * Returns: @a translated in the y-direction by @add_y
 */
extern CoArray
coarray_add_y(CoArray a, double add_y);

/**
 * coarray_eq:
 * @a1: a #CoArray
 * @a2: another #CoArray
 *
 * Returns: 0 if @a1 and @a2 are equal
 */
extern int
coarray_eq(CoArray a1, CoArray a2);

/**
 * coarray_max_y:
 * @a: a #CoArray
 *
 * Returns: the maximum y value in @a
 */
extern double
coarray_max_y(CoArray a);

/**
 * coarray_min_y:
 * @a: a #CoArray
 *
 * Returns: the minimum y value in @a
 */
extern double
coarray_min_y(CoArray a);

/**
 * coarray_length:
 * @a: a #CoArray
 *
 * Returns: the length of @a
 */
extern int
coarray_length(CoArray a);

/**
 * coarray_get:
 * @a: a #CoArray
 * @i: index
 *
 * Returns a copy of the @i-th coordinate of an array. The returned coordinate
 * is newly created and must be freed with coord_free().
 *
 * Returns: a copy of @i-th coordinate
 */
extern Coordinate
coarray_get(CoArray a, int i);

/**
 * coarray_subarray:
 * @a:   a #CoArray
 * @zlo: low z-value of coordinate range
 * @zhi: high z-value of coordinate range
 *
 * Returns a subset of the coordinates in @a as a new coordinate array. The
 * subset is selected so that the z-values of the coordinates are between @zlo
 * and @zhi. @zlo and @zhi must be within the range of the z values of the
 * coordinates contained in @a, inclusive. The resulting coordinate array is
 * newly created and should be freed with coarray_free() when no longer needed.
 *
 * Returns: a subset of @a
 */
extern CoArray
coarray_subarray(CoArray a, double zlo, double zhi);

/**
 * coarray_subarray_y:
 * @a:   a #CoArray
 * @yhi: high y-value of coordinate range
 *
 * Returns a subset of the coordinates in @a as a new #CoArray. The subset is
 * selected so that the y-values of the coordinates are less than @yhi.
 * Interpolated coordinates are added to the ends of the subarray if @yhi
 * doesn't exactly define the y-values of the first and last coordinates in
 * @a. The resulting #CoArray is newly created and should be freed with
 * coarray_free() when no longer needed.
 *
 * Returns: a subset of @a
 */
extern CoArray
coarray_subarray_y(CoArray a, double y);

/**
 * SECTION: xsproperties.h
 * @short_description: Cross section properties
 * @title: CrossSectionProps
 *
 * Cross section properties
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
 * Hydraulic properties calculated from a subsection or cross section
 */
typedef struct CrossSectionProps *CrossSectionProps;

/**
 * xsp_free:
 * @xsp: a #CrossSectionProps
 *
 * Frees a #CrossSectionProps
 *
 * Returns: nothing
 */
extern void
xsp_free(CrossSectionProps xsp);

/**
 * xsp_get:
 * @xsp:   a #CrossSectionProps
 * @prop:  a #xs_prop
 *
 * Returns: the value of @prop contained in @xsp
 */
extern double
xsp_get(CrossSectionProps xsp, xs_prop prop);


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
 * of the first subsection are coarray_get()( @ca, `0`)->z and
 * @z_roughness[`0`]. The lateral boundaries of the `i`-th subsection are
 * @z_roughness[`i`] and @z_roughness[`i + 1`]. The last subsection is bounded
 * by @z_roughness[ @n_roughness `- 2`] and
 * coarray_get()( @ca, `length - 1`)->z, where `length` is the length of @ca.
 *
 * Returns: a new #CrossSection
 */
extern CrossSection
xs_new(CoArray ca, int n_roughness, double *roughness, double *z_roughness);

/**
 * xs_free:
 * @xs: a #CrossSection
 *
 * Decreases the reference count of @xs. Frees @xs if the reference count is
 * zero.
 *
 * Returns: nothing
 */
extern void
xs_free(CrossSection xs);

/**
 * xs_coarray:
 * @xs: a #CrossSection
 *
 * The resulting coordinate array is newly created and should be freed with
 * coarray_free() after use.
 *
 * Returns: a copy of the coordinate array contained in @xs
 */
CoArray
xs_coarray(CrossSection xs);

/**
 * xs_n_subsections:
 * @xs: a #CrossSection
 *
 * Returns the number of sections in @xs.
 *
 * Returns: number of subsections
 */
extern int
xs_n_subsections(CrossSection xs);

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
 * Returns: nothing
 */
extern void
xs_roughness(CrossSection xs, double *roughness);

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
 * Returns: nothing
 */
extern void
xs_z_roughness(CrossSection xs, double *z_roughness);

/**
 * xs_hydraulic_properties:
 * @xs:  a #CrossSection
 * @h:   depth
 *
 * The returned #CrossSectionProps is newly created and should be freed with
 * xsp_free() after use.
 *
 * Returns: the hydraulic properties calculated by @xs at depth @h
 */
extern CrossSectionProps
xs_hydraulic_properties(CrossSection xs, double h);

/**
 * xs_critical_depth
 * @xs:            a #CrossSection
 * @critical_flow: critical flow value
 * @initial_depth: initial depth for solution
 *
 * Computes critical depth using secant solver. Returns `NAN` if no solution is
 * found.
 *
 * Returns: critical depth computed for @critical_flow
 */
extern double
xs_critical_depth(CrossSection xs, double critical_flow, double initial_depth);

/**
 * xs_normal_depth
 * @xs:            a #CrossSection
 * @normal_flow:   normal flow value
 * @slope:         slope for computing normal depth
 * @initial_depth: initial depth for solution
 *
 * Computes normal depth using secant solver. Returns `NAN` if no solution is
 * found.
 *
 * Returns: normal depth computed for @normal_flow
 */
extern double
xs_normal_depth(CrossSection xs,
                double       normal_flow,
                double       slope,
                double       initial_depth);

#endif
