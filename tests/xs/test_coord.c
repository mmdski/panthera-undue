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

CoordTestData *coord_test_new(double y1, double z1, double y2, double z2,
                              double expected_y, double expected_z);
void coord_test_free(CoordTestData *ct);
void check_coord_values(Coordinate c, double y, double z);
void print_coordtestdata(CoordTestData *ct);

/* coord_new test functions */

void init_coord_new(CoordFixture *cf, CoordTestData test_data) {
    cf->c1 = coord_new(test_data.y1, test_data.z1);
}

void coord_new_setup(CoordFixture *cf, gconstpointer test_data) {
    init_coord_new(cf, *(const CoordTestData *)test_data);
}

void coord_new_teardown(CoordFixture *cf, gconstpointer test_data) {
    coord_free(cf->c1);
    coord_test_free((CoordTestData *)test_data);
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
    CoordTestData coord_test_data = *(const CoordTestData *)test_data;
    init_coord_interp(cf, coord_test_data);
}

void coord_interp_teardown(CoordFixture *cf, gconstpointer test_data) {
    coord_free(cf->c1);
    coord_free(cf->c2);
    coord_test_free((CoordTestData *)test_data);
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

CoordTestData *coord_test_new(double y1, double z1, double y2, double z2,
                              double expected_y, double expected_z) {
    CoordTestData *ct;
    NEW(ct);

    ct->y1 = y1;
    ct->z1 = z1;

    ct->y2 = y2;
    ct->z2 = z2;

    ct->expected_y = expected_y;
    ct->expected_z = expected_z;

    return ct;
}

void coord_test_free(CoordTestData *ct) {
    FREE(ct);
}


void check_coord_values(Coordinate c, double y, double z) {

    int y_is_close = test_is_close(coord_y(c), y, ABS_TOL, REL_TOL);
    int z_is_close = test_is_close(coord_z(c), z, ABS_TOL, REL_TOL);

    g_assert(y_is_close);
    g_assert(z_is_close);
}

void print_coordtestdata(CoordTestData *ct) {
    printf("\n");
    printf("ct->y1 = %f\n", ct->y1);
    printf("ct->z1 = %f\n", ct->z1);

    printf("ct->y2 = %f\n", ct->y2);
    printf("ct->z2 = %f\n", ct->z2);

    printf("ct->expected_y = %f\n", ct->expected_y);
    printf("ct->expected_z = %f\n", ct->expected_z);
}

void add_coord_tests(void) {

    /* test init a new coordinate */
    CoordTestData *positive_test_data = coord_test_new(0.5, 0.75, 0, 0, 0, 0);
    g_test_add("/panthera/xs/coord/new test-positive", CoordFixture,
               positive_test_data, coord_new_setup, test_new,
               coord_new_teardown);

    /* test init a new coordinate, negative coords */
    CoordTestData *negative_test_data =
        coord_test_new(-0.5, -0.75, 0, 0, 0, 0);
    g_test_add("/panthera/xs/coord/new test-negative", CoordFixture,
               negative_test_data, coord_new_setup, test_new,
               coord_new_teardown);

    /* test y, z interpolation of coordinates */
    CoordTestData *interp_test_data = coord_test_new(0, 0, 1, 1, 0.5, 0.5);
    g_test_add("/panthera/xs/coord/interp", CoordFixture,
               interp_test_data, coord_interp_setup, test_interp,
               coord_interp_teardown);
}
