#include "CUnit/Basic.h"
#include <panthera/xscoordinate.h>

void test_xsc_new(void) {

    /* set x and y values */
    double x = 0;
    double y = 0;

    /* create a new cross section coordinate */
    XSCoordinate_T s = xsc_new(x, y);

    /* test cross section coordinate values */
    CU_ASSERT(xsc_x(s) == x);
    CU_ASSERT(xsc_y(s) == y);

    xsc_free(s);
}

CU_ErrorCode add_xsc_suite(void) {

    CU_pSuite pXSCSuite = NULL;

    /* add a suite to the registry */
    pXSCSuite = CU_add_suite("xsc suite", NULL, NULL);
    if (NULL == pXSCSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pXSCSuite, "New xsc test", test_xsc_new)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return CUE_SUCCESS;
}
