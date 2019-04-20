#include "testlib.h"
#include <glib.h>

extern void
add_coarray_tests(void);
extern void
add_crosssection_tests(void);

void
add_xs_tests(void)
{
    add_coarray_tests();
    add_crosssection_tests();
}

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    add_xs_tests();
    return g_test_run();
}
