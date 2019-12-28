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
 * const_manning:
 *
 * Returns the conversion factor k used to convert the Manning coefficient from
 * SI units.
 *
 * Returns: Manning formula conversion factor
 */
extern double
const_manning(void);

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

/**
 * const_set_manning:
 * @k: Manning formula conversion factor
 *
 * Sets the conversion factor k used to convert the Manning coefficient from SI
 * units.
 *
 * Typical values are 1 for SI units and 1.49 for English units.
 *
 * Returns: nothing
 */
extern void
const_set_manning(double k);

#endif
