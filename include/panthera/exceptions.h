#ifndef EXCEPTIONS_INCLUDED
#define EXCEPTIONS_INCLUDED

#include <cii/assert.h>
#include <cii/except.h>

/**
 * SECTION: exceptions.h
 * @short_description: Exceptions
 * @title: Exceptions
 *
 * Exception declarations
 *
 */

/**
 * null_ptr_arg_Error:
 *
 * Exception raised when a `NULL` pointer is passed as an invalid argument to
 * a function.
 */
extern const Except_T null_ptr_arg_Error;

/**
 * value_arg_Error:
 *
 * Exception raised when an invalid value is passed in an argument to a
 * function.
 */
extern const Except_T value_arg_Error;

/**
 * coarray_n_coords_Error:
 *
 * Exception raised when there are too few coordinates passed to coarray_new()
 */
extern const Except_T coarray_n_coords_Error;

/**
 * coarray_y_order_Error:
 *
 * Exception raised when the order of @z passed to coarray_new() is incorrect
 */
extern const Except_T coarray_z_order_Error;

/**
 * index_Error:
 *
 * Exception raised when an invalid index value is used to retrieve array
 * values.
 */
extern const Except_T index_Error;

/**
 *xsp_depth_Error:
 *
 * Raised when a depth less than the minimum y-value of a cross section is
 * passed to xs_hydraulic_properties().
 */
extern const Except_T xsp_depth_Error;

/**
 * empty_table_Error:
 *
 * Raised when an unsupported operation is performed on an empty table.
 */
extern const Except_T empty_table_Error;

/**
 * reach_x_order_Error:
 *
 * Raised when the order fo @x passed to xstable_new() is incorrect
 */
extern const Except_T reach_x_order_Error;

/**
 * reach_xs_num_Error:
 *
 * Raised when an invalid cross section number is encountered by a #Reach
 */
extern const Except_T reach_xs_num_Error;

/**
 * max_iterations_Error:
 *
 * Raised when a solver has reached the maximum number of iterations without
 * reaching a solution
 */
extern const Except_T max_iteration_Error;

#endif
