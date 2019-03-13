#include "panthera_test.h"
extern void add_coord_tests(void);
extern void add_subsection_tests(void);

int main(int argc, char **argv) {

    g_test_init(&argc, &argv, NULL);
    add_coord_tests();
    add_subsection_tests();
    return g_test_run();
}
