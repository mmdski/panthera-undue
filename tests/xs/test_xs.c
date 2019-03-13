#include "test_coord.h"

int main(int argc, char **argv) {

    g_test_init(&argc, &argv, NULL);
    add_coord_tests();
    return g_test_run();
}
