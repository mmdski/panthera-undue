#ifndef XSPROPERTIES_INCLUDED
#define XSPROPERTIES_INCLUDED

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

#endif
