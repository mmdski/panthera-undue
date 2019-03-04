#include "CUnit/Basic.h"
#include <panthera/station.h>

void test_station_new(void) {

    /* set x and y values */
    double x = 0;
    double y = 0;

    /* create a new station */
    Station_T s = station_new(x, y);

    /* test station values */
    CU_ASSERT(station_x(s) == x);
    CU_ASSERT(station_y(s) == y);

    station_free(s);
}

CU_ErrorCode add_station_suite(void) {

    CU_pSuite pStationSuite = NULL;

    /* add a suite to the registry */
    pStationSuite = CU_add_suite("station suite", NULL, NULL);
    if (NULL == pStationSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL ==
        CU_add_test(pStationSuite, "New station test", test_station_new)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    return CUE_SUCCESS;
}
