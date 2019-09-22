#ifndef XSPROPERTIES_INCLUDED
#define XSPROPERTIES_INCLUDED

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
 * xsp_new:
 *
 * Creates a new cross section properties. The returned cross section
 * properties is newly allocated and must be freed with xsp_free().
 *
 * Returns: a new #CrossSectionProps
 */
extern CrossSectionProps
xsp_new(void);

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
 * xsp_set:
 * @xsp: a #CrossSectionProps
 * @prop: a #xs_prop to set
 * @value: the value of @prop to set in @xsp
 *
 * Sets the value of the property @prop in @xsp to @value.
 *
 * Returns: nothing
 */
extern void
xsp_set(CrossSectionProps xsp, xs_prop prop, double value);

/**
 * xsp_interp_depth:
 * @xsp1: a #CrossSectionProps
 * @xsp2: another #CrossSectionProps
 * @depth: a depth to interpolate properties
 *
 * Interpolates cross section properties between @xsp1 and @xsp2. The depth of
 * @xsp1 must be less than or equal to the depth of @xsp2. The returned cross
 * section properties is newly allocated and must be freed wth xsp_free() when
 * no longer in use.
 *
 * Returns: interpolated cross section properties
 */
extern CrossSectionProps
xsp_interp_depth(CrossSectionProps xsp1, CrossSectionProps xsp2, double depth);

#endif
