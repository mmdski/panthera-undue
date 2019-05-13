#ifndef EXCEPTIONS_INCLUDED
#define EXCEPTIONS_INCLUDED

#include <panthera/cii/assert.h>
#include <panthera/cii/except.h>

/**
 * SECTION: exceptions.h
 * @short_description: Exceptions
 * @title: Exceptions
 *
 * Exception declarations
 *
 */

/**
 * null_ptr_arg_error:
 *
 * Exception raised when a `NULL` pointer is passed as an invalid argument to
 * a function.
 */
extern const Except_T null_ptr_arg_error;

/**
 * value_arg_error:
 *
 * Exception raised when an invalid value is passed in an argument to a
 * function.
 */
extern const Except_T value_arg_error;

/**
 * coarray_n_coords_error:
 *
 * Exception raised when there are too few coordinates passed to coarray_new()
 */
extern const Except_T coarray_n_coords_error;

/**
 * coarray_z_order_error:
 *
 * Exception raised when the order of @z passed to coarray_new() is incorrect
 */
extern const Except_T coarray_z_order_error;

/**
 * index_error:
 *
 * Exception raised when an invalid index value is used to retrieve array
 * values.
 */
extern const Except_T index_error;

/**
 *xsp_depth_error:
 *
 * Raised when a depth less than the minimum y-value of a cross section is
 * passed to xs_hydraulic_properties().
 */
extern const Except_T xsp_depth_error;

/**
 * empty_table_error:
 *
 * Raised when an unsupported operation is performed on an empty table.
 */
extern const Except_T empty_table_error;

/**
 * reach_x_order_error:
 *
 * Raised when the order fo @x passed to xstable_new() is incorrect
 */
extern const Except_T reach_x_order_error;

/**
 * reach_xs_num_error:
 *
 * Raised when an invalid cross section number is encountered by a #Reach
 */
extern const Except_T reach_xs_num_error;

/**
 * max_iteration_error:
 *
 * Raised when a solver has reached the maximum number of iterations without
 * reaching a solution
 */
extern const Except_T max_iteration_error;

/**
 * compute_fail_error:
 *
 * Raise if a critical error occurs during computation
 */
extern Except_T compute_fail_error;

#endif
