#ifndef CONSTANTS_INCLUDED
#define CONSTANTS_INCLUDED

/**
 * SECTION: constants.h
 * @short_description: Physical constants
 * @title: Constants
 *
 * Application wide access to physical constants
 */

/**
 * const_gravity:
 *
 * Returns the acceleration due to gravity [L/T^2]
 *
 * Returns: acceleration due to gravity
 */
extern double
const_gravity(void);

/**
 * const_set_gravity:
 * @gravity: acceleration due to gravity
 *
 * Sets the acceleration due to gravity [L/T^2]
 *
 * Returns: nothing
 */
extern void
const_set_gravity(double gravity);

#endif
