#include <panthera/reach.h>
#include <panthera/xstable.h>
#include <glib.h>
#include "testlib.h"

void test_reach_new(void) {
    int n_nodes        = 5;
    double x[]         = {0, 1, 2, 3, 4};
    double x_error[]   = {0, 2, 1, 5, 3};
    double y[]         = {0, 0.001, 0.002, 0.003, 0.004};
    int xs_number[]    = {0, 0, 0, 0, 0};
    int xs_num_error[] = {0, 0, 1, 0, 0};

    CrossSection xs = new_cross_section();
    XSTable xstable = xstable_new();
    xstable_put(xstable, 0, xs);

    /* test successful initialization of a reach */
    Reach reach = reach_new(n_nodes, x, y, xs_number, xstable);
    reach_free(reach);
    reach = NULL;

    /* failed init: bad n_nodes */
    TRY
        Reach reach = reach_new(0, x, y, xs_number, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(value_arg_Error);
        ;
    END_TRY;

    /* failed init: null x */
    TRY
        Reach reach = reach_new(n_nodes, NULL, y, xs_number, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* failed init: null y */
    TRY
        Reach reach = reach_new(n_nodes, x, NULL, xs_number, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* failed init: null xs_number */
    TRY
        Reach reach = reach_new(n_nodes, x, y, NULL, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* failed init: null xstable */
    TRY
        Reach reach = reach_new(n_nodes, NULL, y, xs_number, NULL);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* failed init: invalid xs number */
    TRY
        Reach reach = reach_new(n_nodes, x, y, xs_num_error, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(reach_xs_num_Error);
        ;
    END_TRY;

    /* failed init: incorrect x order */
    TRY
        Reach reach = reach_new(n_nodes, x_error, y, xs_number, xstable);
        g_assert_not_reached();
        reach_free(reach); /* to keep the compiler from complaining */
    EXCEPT(reach_x_order_Error);
        ;
    END_TRY;

    xstable_free(xstable);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/panthera/reach/new", test_reach_new);
    return g_test_run();
}
