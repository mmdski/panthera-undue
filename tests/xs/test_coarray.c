#include "cii/mem.h"
#include "coarray.h"
#include "testlib.h"
#include <stdbool.h>
#include <stdio.h>

#define ABS_TOL 1e-10
#define REL_TOL 0

/* no exception placeholder */
const Except_T no_Error;

typedef struct {
    CoArray ca;
    const Except_T *caught_exception;
} CoArrayFixture;

typedef struct CoArrayTestData {
    int n1;
    double *y1;
    double *z1;

    double ylo;
    double yhi;

    CoArray expected;

    const Except_T *exception;
} CoArrayTestData;

CoArrayTestData *new_coarray_test_data(int n1, double *y1, double *z1,
                                       double ylo, double yhi,
                                       CoArray expected,
                                       const Except_T *exception);
void coarray_test_free(CoArrayTestData *cat);
void check_coarray_values(CoArray ca, double *y, double *z);

/* coarray_new test functions */

void init_coarray_new(CoArrayFixture *caf, CoArrayTestData test_data) {
    caf->caught_exception = &no_Error;
    TRY
        caf->ca = coarray_new(test_data.n1, test_data.y1, test_data.z1);
    EXCEPT(coarray_y_order_Error)
        caf->caught_exception = &coarray_y_order_Error;
    END_TRY;
}

void coarray_new_setup(CoArrayFixture *caf, gconstpointer test_data) {
    init_coarray_new(caf, *(const CoArrayTestData *)test_data);
}

void coarray_new_teardown(CoArrayFixture *caf, gconstpointer test_data) {
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

/* test utility functions */

CoArrayTestData *new_coarray_test_data(int n1, double *y1, double *z1,
                                       double ylo, double yhi,
                                       CoArray expected,
                                       const Except_T *exception) {
    CoArrayTestData *cat;
    NEW(cat);

    cat->n1 = n1;
    if (n1 > 0) {
        cat->y1 = Mem_calloc(n1, sizeof(double), __FILE__, __LINE__);
        cat->z1 = Mem_calloc(n1, sizeof(double), __FILE__, __LINE__);
    } else {
        cat->y1 = NULL;
        cat->z1 = NULL;
    }

    cat->ylo = ylo;
    cat->yhi = yhi;


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

void add_coarray_tests(void) {

    int n1 = 4;
    double y1[] = {0, 0.25, 0.5, 1};
    double z1[] = {1, 1.5, 1.25, 1};
    CoArrayTestData *test_data_1 =  new_coarray_test_data(n1, y1, z1, NAN, NAN,
                                                          NULL, &no_Error);
    g_test_add("/panthera/xs/coarray/new/success", CoArrayFixture,
               test_data_1, coarray_new_setup, coarray_test_new,
               coarray_new_teardown);
}
