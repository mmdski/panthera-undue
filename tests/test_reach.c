#include <panthera/reach.h>
#include "testlib.h"

CrossSection new_cross_section(void) {

    int n_coords = 5;
    double y[]   = {1, 0, 0, 0, 1};
    double z[]   = {0, 0, 0.5, 1, 1};

    double n_roughness      = 1;
    double roughness[]      = {0.03};
    double *z_roughness     = NULL;

    CoArray ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    return xs;
}

void test_reach_new(void) {
    Reach reach = reach_new();
    g_assert_true(reach_size(reach) == 0);
    g_assert_true(reach_get(reach, 0) == NULL);
    reach_free(reach);
}

void test_reach_put(void) {
    double x = 0;

    Reach reach = reach_new();
    CrossSection xs = new_cross_section();
    reach_put(reach, x, xs);
    g_assert_true(reach_size(reach) == 1);

    /* test that an error is raised when a null reach is passed to put */
    TRY
        reach_put(NULL, x, xs);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* test that an error is raised when a null cross section is passed to put
     */
    TRY
        reach_put(reach, x, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    reach_free(reach);
}

void test_reach_put_mulit_xs(void) {
    int i;
    int n_xs = 5;
    double x[] = {0, 1, 2, 3, 4, 5};

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        CrossSection xs = new_cross_section();
        reach_put(reach, x[i], xs);
    }

    g_assert_true(reach_size(reach) == n_xs);

    reach_free(reach);
}

void test_reach_get(void) {
    double x     = 0;
    double not_x = 1;

    Reach reach = reach_new();
    CrossSection xs = new_cross_section();
    reach_put(reach, x, xs);
    CrossSection xs1 = reach_get(reach, x);
    g_assert_true(xs == xs1);

    CrossSection xs2 = reach_get(reach, not_x);
    g_assert_true(xs2 == NULL);

    /* test that an error is raised when a null reach is passed to get */
    TRY
        reach_get(NULL, x);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    reach_free(reach);
}

void test_reach_get_multi_xs(void) {
    int i;
    int n_xs = 5;
    double x[] = {0, 1, 2, 3, 4};

    CrossSection  xs;
    CrossSection *xs_in_reach = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_reach + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        reach_put(reach, x[i], *(xs_in_reach + i));
    }

    for (i = 0; i < n_xs; i++) {
        xs = reach_get(reach, x[i]);
        g_assert_true(xs == *(xs_in_reach + i));
    }

    reach_free(reach);
    Mem_free(xs_in_reach, __FILE__, __LINE__);
}


void test_reach_delete(void) {
    int i;
    int n_xs = 5;
    double x[] = {0, 1, 2, 3, 4};

    CrossSection  xs;
    CrossSection *xs_in_reach = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_reach + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        reach_put(reach, x[i], *(xs_in_reach + i));
    }

    /* test deleting non-existent node */
    reach_delete(reach, 10);
    g_assert_true(reach_size(reach) == n_xs);

    for (i = 0; i < n_xs; i++) {
        reach_delete(reach, x[i]);
        xs = reach_get(reach, x[i]);
        g_assert_true(xs == NULL);
        g_assert_true(reach_size(reach) == n_xs - (i + 1));
    }

    TRY
        reach_delete(NULL, x[0]);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    reach_free(reach);
    Mem_free(xs_in_reach, __FILE__, __LINE__);
}

/* test random add and deletes of cross sections from a reach */
void test_reach_delete_random(void) {
    int i;
    int n_xs = 100;
    double random_x;
    double *x_array = NULL;

    CrossSection  xs;

    Reach reach = reach_new();

    while (reach_size(reach) < n_xs) {
        random_x = (float)rand()/(float)(RAND_MAX/1e3);
        xs = new_cross_section();
        reach_put(reach, random_x, xs);
    }

    n_xs = reach_stream_distance(reach, &x_array);
    while (reach_size(reach) > n_xs / 2) {
        i = rand() % n_xs;
        reach_delete(reach, x_array[i]);
    }
    Mem_free(x_array, __FILE__, __LINE__);
    x_array = NULL;

    while (reach_size(reach) < 10 * n_xs) {
        random_x = (float)rand()/(float)(RAND_MAX/1e3);
        xs = new_cross_section();
        reach_put(reach, random_x, xs);
    }

    n_xs = reach_stream_distance(reach, &x_array);
    while (reach_size(reach) > n_xs) {
        i = rand() % n_xs;
        reach_delete(reach, x_array[i]);
    }
    Mem_free(x_array, __FILE__, __LINE__);

    reach_free(reach);
}

void test_reach_stream_distance(void) {
    int i;
    int n_xs = 5;
    double x[] = {0, 1, 2, 3, 4};

    /* for calling reach_stream_distance */
    int n_sd;
    double *stream_distance = NULL;

    CrossSection *xs_in_reach = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_reach + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        reach_put(reach, x[i], *(xs_in_reach + i));
    }

    n_sd = reach_stream_distance(reach, &stream_distance);

    g_assert_true(n_sd == n_xs);

    for (i = 0; i < n_xs; i++) {
        g_assert_true(*(stream_distance + i) == x[i]);
    }

    TRY
        reach_stream_distance(NULL, &stream_distance);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    TRY
        reach_stream_distance(reach, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    TRY
        reach_stream_distance(NULL, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    reach_free(reach);
    Mem_free(xs_in_reach, __FILE__, __LINE__);
    Mem_free(stream_distance, __FILE__, __LINE__);
}

void test_reach_stream_distance_unordered(void) {
    int i;
    int n_xs           = 5;
    double x[]         = {4, 0, 2, 1, 3};
    double x_ordered[] = {0, 1, 2, 3, 4};

    /* for calling reach_stream_distance */
    int n_sd;
    double *stream_distance = NULL;

    CrossSection *xs_in_reach = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_reach + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        reach_put(reach, x[i], *(xs_in_reach + i));
    }

    n_sd = reach_stream_distance(reach, &stream_distance);

    g_assert_true(n_sd == n_xs);

    for (i = 0; i < n_xs; i++) {
        g_assert_true(*(stream_distance + i) == x_ordered[i]);
    }

    TRY
        reach_get_index(NULL, x[0]);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    reach_free(reach);
    Mem_free(xs_in_reach, __FILE__, __LINE__);
    Mem_free(stream_distance, __FILE__, __LINE__);
}

void test_reach_stream_distance_random(void) {
    int i;
    int n_xs = 500;
    double x;

    /* for calling reach_stream_distance */
    int n_sd;
    double *stream_distance = NULL;

    CrossSection *xs_in_reach = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    Reach reach = reach_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_reach + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        x = (float)rand()/(float)(RAND_MAX/1e3);
        reach_put(reach, x, *(xs_in_reach + i));
    }

    n_sd = reach_stream_distance(reach, &stream_distance);

    g_assert_true(n_sd == n_xs);

    for (i = 1; i < n_xs; i++) {
        g_assert_true(stream_distance[i-1] <= stream_distance[i]);
    }

    reach_free(reach);
    Mem_free(xs_in_reach, __FILE__, __LINE__);
    Mem_free(stream_distance, __FILE__, __LINE__);
}

void test_reach_get_index(void) {
    int i;
    int n_xs = 50;
    double x;

    CrossSection xs;
    CrossSection xs_get_index;
    ReachNode rn;

    Reach reach = reach_new();

    /* for calling reach_stream_distance */
    int n_sd;
    double *stream_distance = NULL;

    for (i = 0; i < n_xs; i++) {
        x = (float)rand()/(float)(RAND_MAX/1e3);
        xs = new_cross_section();
        reach_put(reach, x, xs);
    }

    n_sd = reach_stream_distance(reach, &stream_distance);

    for (i = 0; i < n_sd; i++) {
        rn = reach_get_index(reach, i);

        x = reach_node_x(rn);
        g_assert_true(x == *(stream_distance + i));

        xs = reach_get(reach, x);
        xs_get_index = reach_node_xs(rn);
        g_assert_true(xs_get_index == xs);
    }

    reach_free(reach);
    Mem_free(stream_distance, __FILE__, __LINE__);
}

/* test reach_get_index before and after node deletions and more puts */
void test_reach_get_index_put_del(void) {
    int i;
    int n_xs = 50;
    double x;

    CrossSection xs;
    CrossSection xs_get_index;
    ReachNode rn;

    Reach reach = reach_new();

    /* for calling reach_stream_distance */
    int n_sd;
    double *stream_distance = NULL;

    /* fill the reach */
    for (i = 0; i < n_xs; i++) {
        x = (float)rand()/(float)(RAND_MAX/1e3);
        xs = new_cross_section();
        reach_put(reach, x, xs);
    }

    /* get stream distances and test the correctness of the results */
    n_sd = reach_stream_distance(reach, &stream_distance);
    for (i = 0; i < n_sd; i++) {
        rn = reach_get_index(reach, i);
        x = reach_node_x(rn);
        g_assert_true(x == *(stream_distance + i));
        xs = reach_get(reach, x);
        xs_get_index = reach_node_xs(rn);
        g_assert_true(xs_get_index == xs);
    }

    /* delete half of the nodes */
    while (reach_size(reach) > n_xs / 2) {
        i = rand() % n_xs;
        reach_delete(reach, stream_distance[i]);
    }

    Mem_free(stream_distance, __FILE__, __LINE__);
    stream_distance = NULL;

    /* get stream distances and test the correctness of the results again */
    n_sd = reach_stream_distance(reach, &stream_distance);
    for (i = 0; i < n_sd; i++) {
        rn = reach_get_index(reach, i);
        x = reach_node_x(rn);
        g_assert_true(x == *(stream_distance + i));
        xs = reach_get(reach, x);
        xs_get_index = reach_node_xs(rn);
        g_assert_true(xs_get_index == xs);
    }

    Mem_free(stream_distance, __FILE__, __LINE__);
    stream_distance = NULL;

    /* add more nodes */
    for (i = 0; i < n_xs; i++) {
        x = (float)rand()/(float)(RAND_MAX/1e3);
        xs = new_cross_section();
        reach_put(reach, x, xs);
    }

    /* get stream distances and test the correctness of the results again */
    n_sd = reach_stream_distance(reach, &stream_distance);
    for (i = 0; i < n_sd; i++) {
        rn = reach_get_index(reach, i);
        x = reach_node_x(rn);
        g_assert_true(x == *(stream_distance + i));
        xs = reach_get(reach, x);
        xs_get_index = reach_node_xs(rn);
        g_assert_true(xs_get_index == xs);
    }

    Mem_free(stream_distance, __FILE__, __LINE__);
    reach_free(reach);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/panthera/reach/new",       test_reach_new);
    g_test_add_func("/panthera/reach/put",       test_reach_put);
    g_test_add_func("/panthera/reach/put multi", test_reach_put_mulit_xs);
    g_test_add_func("/panthera/reach/get",       test_reach_get);
    g_test_add_func("/panthera/reach/get multi", test_reach_get_multi_xs);
    g_test_add_func("/panthera/reach/delete",    test_reach_delete);
    g_test_add_func("/panthera/reach/delete random",
                    test_reach_delete_random);
    g_test_add_func("/panthera/reach/stream distance",
                    test_reach_stream_distance);
    g_test_add_func("/panthera/reach/stream distance unordered",
                    test_reach_stream_distance_unordered);
    g_test_add_func("/panthera/reach/stream distance random add",
                    test_reach_stream_distance_random);
    g_test_add_func("/panthera/reach/get index", test_reach_get_index);
    g_test_add_func("/panthera/reach/get index del & put",
                    test_reach_get_index_put_del);
    return g_test_run();
}
