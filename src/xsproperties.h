#ifndef XSPROPERTIES_INCLUDED
#define XSPROPERTIES_INCLUDED

#include <panthera/crosssection.h>

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

extern CrossSectionProps
xsp_copy(CrossSectionProps xsp);

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
