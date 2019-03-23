#ifndef PANTHERA_INCLUDED
#define PANTHERA_INCLUDED

#include "cii/assert.h"
#include "cii/except.h"
#include "cii/mem.h"
#include <math.h>

/**
 * SECTION: panthera.h
 * @short_description: Top level header
 * @title: Panthera
 *
 * Contains declarations widely used in Panthera
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

#endif
