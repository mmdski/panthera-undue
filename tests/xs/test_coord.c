#include "cii/mem.h"
#include "coordinate.h"
#include "testlib.h"
#include <stdio.h>

#define ABS_TOL 1e-10
#define REL_TOL 0

typedef struct {
    Coordinate c1;
    Coordinate c2;
} CoordFixture;

typedef struct CoordTestData {
    double y1;
    double z1;

    double y2;
    double z2;

    double expected_y;
    double expected_z;
} CoordTestData;

CoordTestData interp_test_data = {0,   0,

                                  1,   1,

                                  0.5, 0.5};

void check_coord_values(Coordinate c, double y, double z);

/* coord_new test functions */

void init_coord_new(CoordFixture *cf, CoordTestData test_data) {
    cf->c1 = coord_new(test_data.y1, test_data.z1);
}

void coord_new_setup(CoordFixture *cf, gconstpointer test_data) {
    init_coord_new(cf, *(const CoordTestData *)test_data);
}

void coord_new_teardown(CoordFixture *cf, gconstpointer ignore) {
    coord_free(cf->c1);
}

void test_new(CoordFixture *cf, gconstpointer test_data) {
    CoordTestData data = *(const CoordTestData *)test_data;
    check_coord_values(cf->c1, data.y1, data.z1);
}

/* coord_interp_ test_functions */

void init_coord_interp(CoordFixture *cf, CoordTestData test_data) {
    cf->c1 = coord_new(test_data.y1, test_data.z1);
    cf->c2 = coord_new(test_data.y2, test_data.z2);
}

void coord_interp_setup(CoordFixture *cf, gconstpointer test_data) {
    init_coord_interp(cf, *(const CoordTestData *)test_data);
}

void coord_interp_teardown(CoordFixture *cf, gconstpointer ignore) {
    coord_free(cf->c1);
    coord_free(cf->c2);
}

/* general functions */

void test_interp(CoordFixture *cf, gconstpointer test_data) {

    Coordinate c_y;
    Coordinate c_z;

    CoordTestData data = *(const CoordTestData *)test_data;

    /* test coord_interp_y */
    c_y = coord_interp_y(cf->c1, cf->c2, data.expected_y);
    check_coord_values(c_y, data.expected_y, data.expected_z);
    coord_free(c_y);

    /* test coord_interp_z */
    c_z = coord_interp_z(cf->c1, cf->c2, data.expected_z);
    check_coord_values(c_z, data.expected_y, data.expected_z);
    coord_free(c_z);
}

void check_coord_values(Coordinate c, double y, double z) {

    int y_is_close = test_is_close(coord_y(c), y, ABS_TOL, REL_TOL);
    int z_is_close = test_is_close(coord_z(c), z, ABS_TOL, REL_TOL);

    g_assert(y_is_close);
    g_assert(z_is_close);
}

void add_coord_tests(void) {
    CoordTestData positive_test_data = {0.5, 0.75, 0, 0, 0, 0};
    CoordTestData negative_test_data = {-0.5, -0.75, 0, 0, 0, 0};
    g_test_add("/panthera/xs/coord/new test-positive", CoordFixture,
               &positive_test_data, coord_new_setup, test_new,
               coord_new_teardown);
    g_test_add("/panthera/xs/coord/new test-negative", CoordFixture,
               &negative_test_data, coord_new_setup, test_new,
               coord_new_teardown);

    g_test_add("/panthera/xs/coord/new interp", CoordFixture,
               &interp_test_data, coord_interp_setup, test_interp,
               coord_interp_teardown);
}
