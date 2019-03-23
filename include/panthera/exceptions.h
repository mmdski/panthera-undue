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
 * Exception raised when the order of @y passed to coarray_new() is incorrect
 */
extern const Except_T coarray_y_order_Error;

/**
 * index_Error:
 *
 * Exception raised when an invalid index value is used to retrieve array
 * values.
 */
extern const Except_T index_Error;

#endif
