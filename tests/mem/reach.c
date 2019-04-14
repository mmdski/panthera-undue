#include <panthera/reach.h>
#include <stddef.h>
#include "testlib.h"

int main(void) {
    int n_nodes = 5;

    double x[]      = {1, 2, 3, 4, 5};
    double y[]      = {0.01, 0.02, 0.03, 0.04, 0.05};
    int xs_number[] = {0, 0, 0, 0, 0};

    CrossSection xs;
    XSTable xstable;

    Reach reach;

    xs = new_cross_section();
    xstable = xstable_new();

    xstable_put(xstable, 0, xs);

    reach = reach_new(n_nodes, x, y, xs_number, xstable);

    reach_free(reach);
    xstable_free(xstable);
}
