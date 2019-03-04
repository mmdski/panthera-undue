#include "CUnit/Basic.h"
#include <panthera/xscarray.h>

void test_xscarray_new(void) {

    /* set values */
    int n      = 10;
    double x[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    double y[] = {1, 0.75, 0.5, 0.25, 0, 0, 0.25, 0.5, 0.75, 1};

    /* arrays for results */
    double x_result[n];
    double y_result[n];

    /* create a new array */
    XSCArray_T xscarray = xscarray_new(n, x, y);

    /* get the x and y arrays */
    xscarray_x(xscarray, x_result);
    xscarray_y(xscarray, y_result);

    CU_ASSERT(xscarray != NULL);
    CU_ASSERT(xscarray_n(xscarray) == n);

    for (int i = 0; i < n; i++) {
        CU_ASSERT(x_result[i] == x[i]);
        CU_ASSERT(y_result[i] == y[i]);
    }

    xscarray_free(xscarray);
}

CU_ErrorCode add_xscarray_suite(void) {

    CU_pSuite pXSCArraySuite = NULL;

    /* add a suite to the registry */
    pXSCArraySuite = CU_add_suite("xscarray suite", NULL, NULL);
    if (NULL == pXSCArraySuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL ==
        CU_add_test(pXSCArraySuite, "New xscarray test", test_xscarray_new)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return CUE_SUCCESS;
}
