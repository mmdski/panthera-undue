#include "testlib.h"
#include <panthera/cii/mem.h>
#include <panthera/reach.h>
#include <stddef.h>

int
main (void)
{
    int i, j;
    int n_nodes = 5;

    double x[]         = { 1, 2, 3, 4, 5 };
    double y[]         = { 0.01, 0.02, 0.03, 0.04, 0.05 };
    int    xs_number[] = { 0, 0, 0, 0, 0 };

    double h = 0.5;
    double q = 0.1;
    double wse;

    double property;

    double *stream_distance =
        Mem_calloc (n_nodes, sizeof (double), __FILE__, __LINE__);

    CrossSection xs;
    XSTable      xstable;

    Reach reach;

    ReachNodeProps rnp;

    xs      = new_cross_section ();
    xstable = xstable_new ();

    xstable_put (xstable, 0, xs);

    reach = reach_new (n_nodes, x, y, xs_number, xstable);

    for (i = 0; i < n_nodes; i++) {
        wse = h + y[i];
        rnp = reach_node_properties (reach, i, wse, q);
        for (j = 0; j < N_RN; j++) {
            property = rnp_get (rnp, j);
            property += 1; /* keep the compiler from complaining */
        }
        rnp_free (rnp);
    }

    reach_stream_distance (reach, stream_distance);

    Mem_free (stream_distance, __FILE__, __LINE__);
    reach_free (reach);
    xstable_free (xstable);

    return 0;
}
