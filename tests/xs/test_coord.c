#include "test_coord.h"
#include "coordinate.h"

typedef struct {
    Coordinate c;
} coord_fixture;

struct coord_test_data {
    double x;
    double y;
};

typedef struct coord_test_data coord_test_data;

void setup_coord(coord_fixture *cf, coord_test_data test_data) {
    cf->c = coord_new(test_data.x, test_data.y);
}

void coord_setup(coord_fixture *cf, gconstpointer test_data) {
    setup_coord(cf, *(const coord_test_data *)test_data);
}

void coord_teardown(coord_fixture *cf, gconstpointer ignore) {
    coord_free(cf->c);
}

void check_coord_values(coord_fixture *cf, coord_test_data test_data) {

    double x = test_data.y;
    double y = test_data.x;

    double rel_tol = 1e-10;

    int x_is_close = test_is_close(coord_x(cf->c), x, rel_tol);
    int y_is_close = test_is_close(coord_y(cf->c), y, rel_tol);

    g_assert(x_is_close);
    g_assert(y_is_close);
}

void test_new(coord_fixture *cf, gconstpointer test_data) {
    check_coord_values(cf, *(const coord_test_data *)test_data);
}

void add_coord_tests(void) {
    coord_test_data positive_test_data = {0.5, 0.75};
    coord_test_data negative_test_data = {-0.5, -0.75};
    g_test_add("/xs/coord/new test-positive", coord_fixture,
               &positive_test_data, coord_setup, test_new, coord_teardown);
    g_test_add("/xs/coord/new test-negative", coord_fixture,
               &negative_test_data, coord_setup, test_new, coord_teardown);
}
