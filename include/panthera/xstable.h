#ifndef XSTABLE_INCLUDED
#define XSTABLE_INCLUDED

#include <panthera/crosssection.h>
#include <stdbool.h>

/**
 * SECTION: xstable.h
 * @short_description: Cross section table
 * @title: XSTable
 *
 * Cross section table
 *
 * Table containing cross sections and cross section numbers
 */

/**
 * XSTable:
 *
 * Cross section symbol table
 */
typedef struct XSTable *XSTable;

/**
 * xstable_new:
 *
 * Creates a new cross section symbol table. The resulting table is newly
 * created and must be freed with xstable_free().
 *
 * Returns: a new symbol table
 */
extern XSTable
xstable_new(void);

/**
 * xstable_free:
 * @xstable: a #XSTable
 *
 * Frees a cross section symbol table. All cross sections contained in the
 * table are also freed.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable is `NULL`
 *
 * Returns: None
 */
extern void
xstable_free(XSTable xstable);

/**
 * xstable_size:
 * @xstable: a #XSTable
 *
 * Returns the number of cross sections contained in @xstable.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable is `NULL`
 *
 * Returns: The size of @xstable
 */
extern int
xstable_size(XSTable xstable);

/**
 * xstable_get:
 * @xstable: a #XSTable
 * @key:     cross section number
 *
 * Returns the cross section associated with @key. If no cross section is
 * associated with @key, then this function returns `NULL`. @xstable maintains
 * a reference to the returned cross section.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable is `NULL`
 *
 * Returns: Cross section in @xstable associated with @key
 */
extern CrossSection
xstable_get(XSTable xstable, int key);

/**
 * xstable_put:
 * @xstable: a #XSTable
 * @key:     cross section number
 * @xs:      a #CrossSection
 *
 * Associates @xs with the key @key. If a cross section is is already
 * associated with @key in @xstable, then the cross section is freed and @xs is
 * associated with @key. @xstable maintains a reference to @xs.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable or @xs is `NULL`
 *
 * Returns: None
 */
extern void
xstable_put(XSTable xstable, int key, CrossSection xs);

/**
 * xstable_contains:
 * @xstable: a #XSTable
 * @key:     cross section number
 *
 * Returns `true` if @xstable contains a cross section at @key, `false`
 * otherwise.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable is `NULL`
 *
 * Returns: `true` or `false`
 */
extern bool
xstable_contains(XSTable xstable, int key);

/**
 * xstable_delete:
 * @xstable: a #XSTable
 * @key:     cross section number
 *
 * Removes and frees the cross section associated with @key from @xstable.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable or @xs is `NULL`
 *
 * Returns: None
 */
extern void
xstable_delete(XSTable xstable, int key);

/**
 * xstable_keys:
 * @xstable: a #XSTable
 * @keys:    pointer to integer array
 *
 * Allocates memory for @keys and fills the array with keys in @xstable in
 * ascending order. @keys is newly allocated and must be freed.
 *
 * **Raises:**
 *
 * #null_ptr_arg_error if @xstable is `NULL`
 *
 * Returns: the number of elements in @keys
 */
extern int
xstable_keys(XSTable xstable, int **keys);

#endif
