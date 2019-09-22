#ifndef SUBSECTION_INCLUDED
#define SUBSECTION_INCLUDED

#include "xsproperties.h"
#include <panthera/coarray.h>
#include <stdbool.h>

/**
 * SECTION: subsection.h
 * @short_description: Subsection
 * @title: Subsection
 *
 * Cross section subsection
 */

/**
 * SubSection:
 *
 * Subsection of a cross section
 */
typedef struct Subsection *Subsection;

/**
 * subsection_new:
 * @ca: a #CoArray defining the coordinates in the new subsection
 * @roughness: a roughness value for the new subsection
 * @y_activation: a y-value defining the activation of this subsection
 *
 * Creates a new subsection with coordinates defined in @ca and roughness
 * @roughness. @y_activation defines the y-value at which the new subsection
 * will compute cross section properties. The returned subsection is newly
 * created and must be freed with subsection_free().
 *
 * Returns: a new subsection
 */
extern Subsection
subsection_new(CoArray ca, double roughness, double y_activation);

/**
 * subsection_free:
 * @ss: a #Subsection
 *
 * Frees a subsection.
 *
 * Returns: nothing
 */
extern void
subsection_free(Subsection ss);

/**
 * subsection_properties:
 * @ss: a #Subsection
 * @y: a y-value for computing properties
 *
 * Computes properties for a subsection at a y value.
 *
 * Returns: cross section properties
 */
extern CrossSectionProps
subsection_properties(Subsection ss, double y);

/**
 * subsection_roughness:
 * @ss: a #Subsection
 *
 * Returns the roughness value of subsection @ss.
 *
 * Returns: a roughness value
 */
extern double
subsection_roughness(Subsection ss);

/**
 * subsection_activated:
 * @ss: a #Subsection
 * @y: vertical value
 *
 * Returns: true if @ss is actviated at vertical value @y
 */
extern bool
subsection_activated(Subsection ss, double y);

/**
 * subsection_z:
 * @ss: a #Subsection
 *
 * Returns: maximum z-value in subsection
 */
extern double
subsection_z(Subsection ss);

#endif
