#ifndef LIST_INCLUDED
#define LIST_INCLUDED

/**
 * SECTION: list.h
 * @short_description: Linked list
 * @title: List
 *
 * Linked list interface
 */

/**
 * List:
 *
 * Linked list
 */
typedef struct List *List;

/**
 * list_new:
 *
 * Creates an empty list. The returned list is newly created and must be freed
 * with list_free().
 *
 * Returns: nothing
 */
extern List
list_new(void);

/**
 * list_free:
 * @list: a #List
 *
 * Frees a linked list.
 *
 * Returns: nothing
 */
extern void
list_free(List list);

/**
 * list_append:
 * @list: a #List
 * @item: an item to append to @list
 *
 * Appends @item to @list.
 *
 * Returns: nothing
 */
extern void
list_append(List list, void *item);

/**
 * list_length:
 * @list: a #List
 *
 * Returns the length of @list.
 *
 * Returns: length of list
 */
extern int
list_length(List list);

/**
 * list_to_array:
 * @list: a #List
 *
 * Returns an array created from the items in @list. The array is newly created
 * and must be freed.
 *
 * Returns: an array of items
 */
extern void *
list_to_array(List list);

#endif
