#ifndef CUNIT_BASIC_H_SEEN
#include "CUnit/Basic.h"
#endif
#include "test_xscoordinate.h"

int main(int argc, char **argv) {

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add station test suite to the registry */
    if (CUE_SUCCESS != add_xsc_suite()) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
