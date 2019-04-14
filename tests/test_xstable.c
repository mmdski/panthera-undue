#include <panthera/xstable.h>
#include "testlib.h"
#include <glib.h>


void test_xstable_new(void) {
    XSTable xstable = xstable_new();
    g_assert_true(xstable_size(xstable) == 0);
    g_assert_true(xstable_get(xstable, 0) == NULL);
    xstable_free(xstable);
}

void test_xstable_put(void) {
    int key = 0;

    XSTable xstable = xstable_new();
    CrossSection xs = new_cross_section();
    xstable_put(xstable, key, xs);
    g_assert_true(xstable_size(xstable) == 1);

    /* test that an error is raised when a null xstable is passed to put */
    TRY
        xstable_put(NULL, key, xs);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    /* test that an error is raised when a null cross section is passed to put
     */
    TRY
        xstable_put(xstable, key, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    xstable_free(xstable);
}

void test_xs_table_put_multi(void) {
    int i;
    int n_xs = 5;
    int keys[] = {0, 1, 2, 3, 4, 5};

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        CrossSection xs = new_cross_section();
        xstable_put(xstable, keys[i], xs);
    }

    g_assert_true(xstable_size(xstable) == n_xs);

    xstable_free(xstable);
}

void test_xstable_get(void) {
    int key     = 0;
    int not_key = 1;

    XSTable xstable = xstable_new();
    CrossSection xs = new_cross_section();
    xstable_put(xstable, key, xs);
    CrossSection xs1 = xstable_get(xstable, key);
    g_assert_true(xs == xs1);

    CrossSection xs2 = xstable_get(xstable, not_key);
    g_assert_true(xs2 == NULL);

    /* test that an error is raised when a null xstable is passed to get */
    TRY
        xstable_get(NULL, key);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    xstable_free(xstable);
}

void test_xstable_get_multi_xs(void) {
    int i;
    int n_xs = 5;
    int keys[] = {0, 1, 2, 3, 4};

    CrossSection  xs;
    CrossSection *xs_in_xstable = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_xstable + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        xstable_put(xstable, keys[i], *(xs_in_xstable + i));
    }

    for (i = 0; i < n_xs; i++) {
        xs = xstable_get(xstable, keys[i]);
        g_assert_true(xs == *(xs_in_xstable + i));
    }

    xstable_free(xstable);
    Mem_free(xs_in_xstable, __FILE__, __LINE__);
}


void test_xstable_delete(void) {
    int i;
    int n_xs = 5;
    int keys[] = {0, 1, 2, 3, 4};

    CrossSection  xs;
    CrossSection *xs_in_xstable = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_xstable + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        xstable_put(xstable, keys[i], *(xs_in_xstable + i));
    }

    /* test deleting non-existent node */
    xstable_delete(xstable, 10);
    g_assert_true(xstable_size(xstable) == n_xs);

    for (i = 0; i < n_xs; i++) {
        xstable_delete(xstable, keys[i]);
        xs = xstable_get(xstable, keys[i]);
        g_assert_true(xs == NULL);
        g_assert_true(xstable_size(xstable) == n_xs - (i + 1));
    }

    TRY
        xstable_delete(NULL, keys[0]);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    xstable_free(xstable);
    Mem_free(xs_in_xstable, __FILE__, __LINE__);
}

/* test random add and deletes of cross sections from a xstable */
void test_xstable_delete_random(void) {
    int i;
    int n_xs = 100;
    int random_key;
    int *key_array = NULL;

    CrossSection  xs;

    XSTable xstable = xstable_new();

    while (xstable_size(xstable) < n_xs) {
        random_key = rand();
        xs = new_cross_section();
        xstable_put(xstable, random_key, xs);
    }

    n_xs = xstable_keys(xstable, &key_array);
    while (xstable_size(xstable) > n_xs / 2) {
        i = rand() % n_xs;
        xstable_delete(xstable, key_array[i]);
    }
    Mem_free(key_array, __FILE__, __LINE__);
    key_array = NULL;

    while (xstable_size(xstable) < 10 * n_xs) {
        random_key = rand();
        xs = new_cross_section();
        xstable_put(xstable, random_key, xs);
    }

    n_xs = xstable_keys(xstable, &key_array);
    while (xstable_size(xstable) > n_xs) {
        i = rand() % n_xs;
        xstable_delete(xstable, key_array[i]);
    }
    Mem_free(key_array, __FILE__, __LINE__);

    xstable_free(xstable);
}

void test_xstable_keys(void) {
    int i;
    int n_xs = 5;
    int keys[] = {0, 1, 2, 3, 4};

    /* for calling xstable_keys */
    int n_sd;
    int *keys_array = NULL;

    CrossSection *xs_in_xstable = Mem_calloc(n_xs, sizeof(CrossSection),
                                             __FILE__, __LINE__);

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_xstable + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        xstable_put(xstable, keys[i], *(xs_in_xstable + i));
    }

    n_sd = xstable_keys(xstable, &keys_array);

    g_assert_true(n_sd == n_xs);

    for (i = 0; i < n_xs; i++) {
        g_assert_true(*(keys_array + i) == keys[i]);
    }

    TRY
        xstable_keys(NULL, &keys_array);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    TRY
        xstable_keys(xstable, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    TRY
        xstable_keys(NULL, NULL);
        g_assert_not_reached();
    EXCEPT(null_ptr_arg_Error);
        ;
    END_TRY;

    xstable_free(xstable);
    Mem_free(xs_in_xstable, __FILE__, __LINE__);
    Mem_free(keys_array, __FILE__, __LINE__);
}

void test_xstable_keys_unordered(void) {
    int i;
    int n_xs              = 5;
    int keys[]         = {4, 0, 2, 1, 3};
    int keys_ordered[] = {0, 1, 2, 3, 4};

    /* for calling xstable_keys */
    int n_sd;
    int *keys_array = NULL;

    CrossSection *xs_in_xstable = Mem_calloc(n_xs, sizeof(CrossSection),
                                             __FILE__, __LINE__);

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_xstable + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        xstable_put(xstable, keys[i], *(xs_in_xstable + i));
    }

    n_sd = xstable_keys(xstable, &keys_array);

    g_assert_true(n_sd == n_xs);

    for (i = 0; i < n_xs; i++) {
        g_assert_true(*(keys_array + i) == keys_ordered[i]);
    }

    xstable_free(xstable);
    Mem_free(xs_in_xstable, __FILE__, __LINE__);
    Mem_free(keys_array, __FILE__, __LINE__);
}

void test_xstable_keys_random(void) {
    int i;
    int n_xs = 500;
    int key;

    /* for calling xstable_keys */
    int n_sd;
    int *keys_array = NULL;

    CrossSection *xs_in_xstable = Mem_calloc(n_xs, sizeof(CrossSection),
                                           __FILE__, __LINE__);

    XSTable xstable = xstable_new();

    for (i = 0; i < n_xs; i++) {
        *(xs_in_xstable + i) = new_cross_section();
    }

    for (i = 0; i < n_xs; i++) {
        key = rand();
        xstable_put(xstable, key, *(xs_in_xstable + i));
    }

    n_sd = xstable_keys(xstable, &keys_array);

    g_assert_true(n_sd == n_xs);

    for (i = 1; i < n_xs; i++) {
        g_assert_true(keys_array[i-1] < keys_array[i]);
    }

    xstable_free(xstable);
    Mem_free(xs_in_xstable, __FILE__, __LINE__);
    Mem_free(keys_array, __FILE__, __LINE__);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/panthera/xstable/new",       test_xstable_new);
    g_test_add_func("/panthera/xstable/put",       test_xstable_put);
    g_test_add_func("/panthera/xstable/put multi", test_xs_table_put_multi);
    g_test_add_func("/panthera/xstable/get",       test_xstable_get);
    g_test_add_func("/panthera/xstable/get multi", test_xstable_get_multi_xs);
    g_test_add_func("/panthera/xstable/delete",    test_xstable_delete);
    g_test_add_func("/panthera/xstable/delete random",
                    test_xstable_delete_random);
    g_test_add_func("/panthera/xstable/keys", test_xstable_keys);
    g_test_add_func("/panthera/xstable/keys unordered",
                    test_xstable_keys_unordered);
    g_test_add_func("/panthera/xstable/keys random add",
                    test_xstable_keys_random);
    return g_test_run();
}
