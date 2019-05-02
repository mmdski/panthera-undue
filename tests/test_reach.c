#include "testlib.h"
#include <glib.h>
#include <panthera/reach.h>
#include <panthera/xstable.h>

void
test_reach_new (void)
{
    int    n_nodes        = 5;
    double x[]            = { 0, 1, 2, 3, 4 };
    double x_error[]      = { 0, 2, 1, 5, 3 };
    double y[]            = { 0, 0.001, 0.002, 0.003, 0.004 };
    int    xs_number[]    = { 0, 0, 0, 0, 0 };
    int    xs_num_error[] = { 0, 0, 1, 0, 0 };

    CrossSection xs      = new_cross_section ();
    XSTable      xstable = xstable_new ();
    xstable_put (xstable, 0, xs);

    /* test successful initialization of a reach */
    Reach reach = reach_new (n_nodes, x, y, xs_number, xstable);

    g_assert_true (reach_size (reach) == n_nodes);

    reach_free (reach);
    reach = NULL;

    /* failed free: null reach */
    TRY reach_free (reach);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed size: null reach */
    TRY n_nodes = reach_size (reach);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed init: bad n_nodes */
    TRY Reach reach = reach_new (0, x, y, xs_number, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (value_arg_error);
    ;
    END_TRY;

    /* failed init: null x */
    TRY Reach reach = reach_new (n_nodes, NULL, y, xs_number, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed init: null y */
    TRY Reach reach = reach_new (n_nodes, x, NULL, xs_number, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed init: null xs_number */
    TRY Reach reach = reach_new (n_nodes, x, y, NULL, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed init: null xstable */
    TRY Reach reach = reach_new (n_nodes, NULL, y, xs_number, NULL);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    /* failed init: invalid xs number */
    TRY Reach reach = reach_new (n_nodes, x, y, xs_num_error, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (reach_xs_num_error);
    ;
    END_TRY;

    /* failed init: incorrect x order */
    TRY Reach reach = reach_new (n_nodes, x_error, y, xs_number, xstable);
    g_assert_not_reached ();
    reach_free (reach); /* to keep the compiler from complaining */
    EXCEPT (reach_x_order_error);
    ;
    END_TRY;

    xstable_free (xstable);
}

void
test_reach_node_props (void)
{
    int    i;
    int    n_nodes     = 5;
    double x[]         = { 0, 1, 2, 3, 4 };
    double y[]         = { 0, 0.001, 0.002, 0.003, 0.004 };
    int    xs_number[] = { 0, 0, 0, 0, 0 };
    double q           = 0.1;
    double h           = 0.5;
    double wse;

    ReachNodeProps rnp;

    CrossSection xs      = new_cross_section ();
    XSTable      xstable = xstable_new ();
    xstable_put (xstable, 0, xs);

    Reach reach = reach_new (n_nodes, x, y, xs_number, xstable);

    for (i = 0; i < n_nodes; i++) {
        wse = h + y[i];
        rnp = reach_node_properties (reach, i, wse, q);
        g_assert_true (rnp_get (rnp, RN_X) == x[i]);
        g_assert_true (rnp_get (rnp, RN_WSE) == wse);
        g_assert_true (rnp_get (rnp, RN_DISCHARGE) == q);
        rnp_free (rnp);
    }

    /* test failure with invalid index */
    TRY reach_node_properties (reach, -1, wse, q);
    g_assert_not_reached ();
    EXCEPT (index_error);
    ;
    END_TRY;

    TRY reach_node_properties (reach, n_nodes, wse, q);
    g_assert_not_reached ();
    EXCEPT (index_error);
    ;
    END_TRY;

    reach_free (reach);
    xstable_free (xstable);

    reach = NULL;

    /* test failure with null reach */
    TRY reach_node_properties (reach, 0, wse, q);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;
}

void
test_reach_stream_distance (void)
{
    int    i;
    int    n_nodes     = 5;
    double x[]         = { 0, 1, 2, 3, 4 };
    double y[]         = { 0, 0.001, 0.002, 0.003, 0.004 };
    int    xs_number[] = { 0, 0, 0, 0, 0 };

    double *stream_distance =
        Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);

    CrossSection xs      = new_cross_section ();
    XSTable      xstable = xstable_new ();
    xstable_put (xstable, 0, xs);

    Reach reach = reach_new (n_nodes, x, y, xs_number, xstable);

    TRY reach_stream_distance (NULL, stream_distance);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    TRY reach_stream_distance (reach, NULL);
    g_assert_not_reached ();
    EXCEPT (null_ptr_arg_error);
    ;
    END_TRY;

    reach_stream_distance (reach, stream_distance);

    for (i = 0; i < n_nodes; i++) {
        g_assert_true (*(stream_distance + i) == *(x + i));
    }

    Mem_free (stream_distance, __FILE__, __LINE__);
    reach_free (reach);
    xstable_free (xstable);
}

int
main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);
    g_test_add_func ("/panthera/reach/new", test_reach_new);
    g_test_add_func ("/panthera/reach/node properties", test_reach_node_props);
    g_test_add_func ("/panthera/reach/stream distance",
                     test_reach_stream_distance);
    return g_test_run ();
}
