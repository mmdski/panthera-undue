#ifndef REACH_INCLUDED
#define REACH_INCLUDED

#include <panthera/crosssection.h>
#include <stdbool.h>

/**
 * SECTION: reach.h
 * @short_description: Reach
 * @title: Reach
 *
 * River reach
 *
 * A reach contains #CrossSection instances and stream distances.
 */

/**
 * ReachNode:
 *
 * Nodes that provide access to stream distances and corresponding cross
 * sections.
 */
typedef struct ReachNode *ReachNode;

/**
 * reach_node_x:
 * @node: a #ReachNode
 *
 * Returns the stream distance in @node.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @node is `NULL`
 *
 * Returns: stream distance of @node
 */
extern double reach_node_x(ReachNode node);

/**
 * reach_node_xs:
 * @node: a #ReachNode
 *
 * Returns a reference to the cross section in @node
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @node is `NULL`
 *
 * Returns: a reference to the cross section in @node
 */
extern CrossSection reach_node_xs(ReachNode node);

/**
 * Reach:
 *
 * Data structure containing river reach simulation information.
 */
typedef struct Reach *Reach;

/**
 * reach_new:
 *
 * Creates a new reach. The resulting reach is newly created and must be freed
 * with reach_free().
 *
 * Returns: a new reach
 */
extern Reach reach_new(void);

/**
 * reach_free:
 * @reach: a #Reach
 *
 * Frees a reach. All cross sections contained in the reach are also freed.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: None
 */
extern void reach_free(Reach reach);

/**
 * reach_size:
 * @reach: a #Reach
 *
 * Returns the number of cross sections contained in @reach.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: The size of @reach
 */
extern int reach_size(Reach reach);

/**
 * reach_get:
 * @reach: a #Reach
 * @x:     distance downstream
 *
 * Returns the cross section associated with the distance downstream x. If
 * no cross section is associated with x, then reach_get() returns `NULL`.
 * @reach maintains a reference to the returned cross section.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: Cross section in @reach associated with @x
 */
extern CrossSection reach_get(Reach reach, double x);

/**
 * reach_put:
 * @reach: a #Reach
 * @x:     distance downstream
 * @xs:    a #CrossSection
 *
 * Associates @xs with the distance downstream @x. If a cross section is
 * is already associated with @x in @reach, then the cross section is freed
 * and @xs is associated with @x. @reach maintains a reference to @xs.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach or @xs is `NULL`
 *
 * Returns: None
 */
extern void reach_put(Reach reach, double x, CrossSection xs);

/**
 * reach_get_index:
 * @reach:  a #Reach
 * @i:     an index
 *
 * Returns the @i-th #ReachNode. @reach maintains a reference to the returned
 * node.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * #index_Error if @i > reach_size()( @reach ) - 1
 *
 * Returns: The @i-th node
 */
extern ReachNode reach_get_index(Reach reach, int i);

/**
 * reach_contains:
 * @reach: a #Reach
 * @x:     distance downstream
 *
 * Returns `true` if @reach contains a cross section at @x, `false` otherwise.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 * Returns: `true` or `false`
 */
extern bool reach_contains(Reach reach, double x);

/**
 * reach_delete:
 * @reach: a #Reach
 * @x:     distance downstream
 *
 * Removes and frees the cross section associated with @x from the reach.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach or @xs is `NULL`
 *
 * Returns: None
 */
extern void reach_delete(Reach reach, double x);

/**
 * reach_stream_distance:
 * @reach: a #Reach
 * @x:     a reference to a double array
 *
 * Inserts the stream distances of cross sections contained in @reach into the
 * array referenced by @x and returns the length of the referenced array. The
 * array referenced by @x is newly allocated and should be freed.
 *
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach or @x is `NULL`
 *
 * Returns: the length of the array referenced by @x
 */
extern int reach_stream_distance(Reach reach, double **x);

/**
 * reach_node_x:
 * @node: a #ReachNode
 *
 * Returns the stream distance of @node.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 */
extern double reach_node_x(ReachNode node);

/**
 * reach_node_xs:
 * @node: a #ReachNode
 *
 * Returns the cross section of @node. @node maintains a reference to the
 * returned cross section.
 *
 * **Raises:**
 *
 * #null_ptr_arg_Error if @reach is `NULL`
 *
 */
extern CrossSection reach_node_xs(ReachNode node);

#endif
