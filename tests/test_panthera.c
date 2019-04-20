#include <glib.h>

extern void
add_xs_tests(void);

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    add_xs_tests();
    return g_test_run();
}
