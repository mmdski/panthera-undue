#include "testlib.h"
extern void add_coord_tests(void);
extern void add_subsection_tests(void);

void add_xs_tests(void) {
    add_coord_tests();
    add_subsection_tests();
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);
    add_xs_tests();
    return g_test_run();
}
