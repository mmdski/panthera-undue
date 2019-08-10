#include "memtest.h"

int
main (void)
{
    test_xs_roughness ();
    test_xs_n_roughness ();
    test_xs_critical_depth ();
    test_xs_normal_depth ();
    test_reach ();
    test_xstable ();
    test_standardstep ();

    return 0;
}
