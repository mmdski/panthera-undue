#include <panthera/coarray.h>
#include <panthera/exceptions.h>
#include <stdbool.h>
#include <stdio.h>
#include "testlib.h"

#define ABS_TOL 1e-10
#define REL_TOL 0

typedef struct {
    CoArray ca;
    const Except_T *caught_exception;
} CoArrayFixture;

typedef struct CoArrayTestData {
    int n1;
    double *y1;
    double *z1;

    double zlo;
    double zhi;

    CoArray expected;

    const Except_T *exception;
} CoArrayTestData;

CoArrayTestData *new_coarray_test_data(int n1, double *y1, double *z1,
                                       double zlo, double zhi,
                                       CoArray expected,
                                       const Except_T *exception);
void coarray_test_free(CoArrayTestData *cat);
void check_coarray_values(CoArray ca, double *y, double *z);
void print_coarray(CoArray ca);

/* coarray_new test functions */

void init_coarray_new(CoArrayFixture *caf, CoArrayTestData test_data) {

    TRY
        caf->ca = coarray_new(test_data.n1, test_data.y1, test_data.z1);
        caf->caught_exception = &no_Error;
    EXCEPT(null_ptr_arg_Error)
        caf->caught_exception = &null_ptr_arg_Error;
    EXCEPT(coarray_n_coords_Error)
        caf->caught_exception = &coarray_n_coords_Error;
    EXCEPT(coarray_z_order_Error)
        caf->caught_exception = &coarray_z_order_Error;
    END_TRY;

}

void coarray_new_setup(CoArrayFixture *caf, gconstpointer test_data) {
    init_coarray_new(caf, *(const CoArrayTestData *)test_data);
}

void coarray_new_teardown(CoArrayFixture *caf, gconstpointer test_data) {
    if (caf->ca)
        coarray_free(caf->ca);
    coarray_test_free((CoArrayTestData *)test_data);
}

void coarray_test_new(CoArrayFixture *caf, gconstpointer test_data) {
    CoArrayTestData data = *(const CoArrayTestData *)test_data;
    const Except_T *caught_exception = caf->caught_exception;
    g_assert_true(caught_exception == data.exception);
    if (data.exception == &no_Error)
        check_coarray_values(caf->ca, data.y1, data.z1);
}

/* coarray_subarray_y test functions */

void coarray_test_subarray_z(CoArrayFixture *caf, gconstpointer test_data) {
    CoArrayTestData data = *(const CoArrayTestData *)test_data;

    CoArray result = coarray_subarray_z(caf->ca, data.zlo, data.zhi);

    g_assert_true(coarray_eq(result, data.expected));
    coarray_free(result);
}

/* coarray_add_z test functions */

void coarray_test_add_y(CoArrayFixture *caf, gconstpointer test_data) {
    CoArrayTestData data = *(const CoArrayTestData *)test_data;

    CoArray result = coarray_add_y(caf->ca, data.zlo);
    g_assert_true(coarray_eq(result, data.expected));
    coarray_free(result);
}

/* test utility functions */

CoArrayTestData *new_coarray_test_data(int n1, double *y1, double *z1,
                                       double zlo, double zhi,
                                       CoArray expected,
                                       const Except_T *exception) {
    int i;

    CoArrayTestData *cat;
    NEW(cat);

    cat->n1 = n1;

    if (n1 > 0 && y1 != NULL) {
        cat->y1 = Mem_calloc(n1, sizeof(double), __FILE__, __LINE__);
        for (i = 0; i < n1; i++)
            *(cat->y1 + i) = *(y1 + i);
    } else {
        cat->y1 = NULL;
    }

    if (n1 > 0 && z1 != NULL) {
        cat->z1 = Mem_calloc(n1, sizeof(double), __FILE__, __LINE__);
        for (i = 0; i < n1; i++)
            *(cat->z1 + i) = *(z1 + i);
    } else {
        cat->z1 = NULL;
    }

    cat->zlo = zlo;
    cat->zhi = zhi;

    cat->expected = expected;
    cat->exception = exception;

    return cat;
}

void coarray_test_free(CoArrayTestData *cat) {

    if (cat->n1 > 0) {
        Mem_free(cat->y1, __FILE__, __LINE__);
        Mem_free(cat->z1, __FILE__, __LINE__);
    }

    if (cat->expected)
        coarray_free(cat->expected);

    FREE(cat);
}

void check_coarray_values(CoArray ca, double *y, double *z) {

    int i;
    int n = coarray_length(ca);

    bool y_is_close;
    bool z_is_close;

    for (i = 0; i < n; i++) {
        y_is_close = test_is_close(coarray_get_y(ca, i), *(y + i), ABS_TOL,
                                   REL_TOL);
        z_is_close = test_is_close(coarray_get_z(ca, i), *(z + i), ABS_TOL,
                                   REL_TOL);
        g_assert_true(y_is_close);
        g_assert_true(z_is_close);
    }
}

void print_coarray(CoArray ca) {
    int i;
    int n = coarray_length(ca);
    printf("\n");
    printf("ca->n = %d\n", n);
    for (i = 0; i < n; i++) {
        printf("i = %d\ty = %f\tz = %f\n", i, coarray_get_y(ca, i),
               coarray_get_z(ca, i));
    }
}

void add_new_tests(void) {

    /* test succesful initialization */
    int n1 = 4;
    double y1[] = {1, 1.5, 1.25, 1};
    double z1[] = {0, 0.25, 0.5, 1};
    CoArrayTestData *test_data_1 =  new_coarray_test_data(n1, y1, z1, NAN, NAN,
                                                          NULL, &no_Error);
    g_test_add("/panthera/xs/coarray/new/success", CoArrayFixture,
               test_data_1, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);

    /* test failed initialization with 1 coordinate */
    int n2 = 1;
    double y2[] = {0};
    double z2[] = {0};
    CoArrayTestData *test_data_2 = new_coarray_test_data(
                                                n2, y2, z2, NAN, NAN, NULL,
                                                &coarray_n_coords_Error);
    g_test_add("/panthera/xs/coarray/new/fail/n_coords", CoArrayFixture,
               test_data_2, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);

    /* test failed initialization with null y */
    CoArrayTestData *test_data_3 = new_coarray_test_data(
                                                n1, NULL, z1, NAN, NAN, NULL,
                                                &null_ptr_arg_Error);
    g_test_add("/panthera/xs/coarray/new/fail/null y", CoArrayFixture,
               test_data_3, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);

    /* test failed initialization with null z */
    CoArrayTestData *test_data_4 = new_coarray_test_data(
                                                n1, y1, NULL, NAN, NAN, NULL,
                                                &null_ptr_arg_Error);
    g_test_add("/panthera/xs/coarray/new/fail/null z", CoArrayFixture,
               test_data_4, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);

    /* test failed initialization with null y, z */
    CoArrayTestData *test_data_5 = new_coarray_test_data(
                                                n1, NULL, NULL, NAN, NAN, NULL,
                                                &null_ptr_arg_Error);
    g_test_add("/panthera/xs/coarray/new/fail/null y, z", CoArrayFixture,
               test_data_5, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);

    /* test failed initialization with out of order y values */
    double z6[] = {0, 0.5, 0.25, 1};
    CoArrayTestData *test_data_6 = new_coarray_test_data(
                                                n1, y1, z6, NAN, NAN, NULL,
                                                &coarray_z_order_Error);
    g_test_add("/panthera/xs/coarray/new/fail/y_order", CoArrayFixture,
               test_data_6, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);
}

void add_subarray_z_tests(void) {

    /* two points inside array */
    int n1 = 4;
    double z1[] = {0, 1, 2, 3};
    double y1[] = {1.5, 1, 1, 1.5};
    double zlo1 = 0.5;
    double zhi1 = 2.5;
    double expected_z1[] = {0.5, 1, 2, 2.5};
    double expected_y1[] = {1.25, 1, 1, 1.25};
    CoArray expected1 = coarray_new(n1, expected_y1, expected_z1);
    CoArrayTestData *test_data_1 = new_coarray_test_data(n1, y1, z1, zlo1,
                                                         zhi1, expected1,
                                                         &no_Error);
    g_test_add("/panthera/xs/coarray/subarray_z/success 1", CoArrayFixture,
               test_data_1, coarray_new_setup, coarray_test_subarray_z,
               coarray_new_teardown);

    /* two points exactly equal to y values */
    int n2 = 2;
    double zlo2 = 1;
    double zhi2 = 2;
    double expected_z2[] = {1, 2};
    double expected_y2[] = {1, 1};
    CoArray expected2 = coarray_new(n2, expected_y2, expected_z2);
    CoArrayTestData *test_data_2 = new_coarray_test_data(n1, y1, z1, zlo2,
                                                         zhi2, expected2,
                                                         &no_Error);
    g_test_add("/panthera/xs/coarray/subarray_z/success 2", CoArrayFixture,
               test_data_2, coarray_new_setup, coarray_test_subarray_z,
               coarray_new_teardown);

    /* two points outside y values */
    double zlo3 = -INFINITY;
    double zhi3 = INFINITY;
    CoArray expected3 = coarray_new(n1, y1, z1);
    CoArrayTestData *test_data_3 = new_coarray_test_data(n1, y1, z1, zlo3,
                                                         zhi3, expected3,
                                                         &no_Error);
    g_test_add("/panthera/xs/coarray/subarray_z/success 3", CoArrayFixture,
               test_data_3, coarray_new_setup, coarray_test_subarray_z,
               coarray_new_teardown);
}

void add_coarray_add_y_tests(void) {
        int n1 = 4;
        double z1[] = {0, 1, 2, 3};
        double y1[] = {1.5, 1, 1, 1.5};
        double zlo1 = -1;
        double zhi1 = 0;
        double expected_y1[] = {0.5, 0, 0, 0.5};
        CoArray expected1 = coarray_new(n1, expected_y1, z1);
        CoArrayTestData *test_data_1 = new_coarray_test_data(n1, y1, z1, zlo1,
                                                             zhi1, expected1,
                                                             &no_Error);
        g_test_add("/panthera/xs/coarray/subtract_y/success", CoArrayFixture,
                   test_data_1, coarray_new_setup, coarray_test_add_y,
                   coarray_new_teardown);
}

void add_coarray_tests(void) {
    add_new_tests();
    add_subarray_z_tests();
    add_coarray_add_y_tests();
}
