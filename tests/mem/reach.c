#include <panthera/reach.h>
#include <stdlib.h>

CrossSection
new_cross_section(void)
{

    int    n_coords = 5;
    double y[]      = { 1, 0, 0, 0, 1 };
    double z[]      = { 0, 0, 0.5, 1, 1 };

    double  n_roughness = 1;
    double  roughness[] = { 0.03 };
    double *z_roughness = NULL;

    CoArray      ca = coarray_new(n_coords, y, z);
    CrossSection xs = xs_new(ca, n_roughness, roughness, z_roughness);
    coarray_free(ca);

    return xs;
}

void
test_reach_new(void)
{
    int    n_nodes = 5;
    double x[]     = { 0, 1, 2, 3, 4 };
    double y[]     = { 0, 0.001, 0.002, 0.003, 0.004 };

    CrossSection xs = new_cross_section();

    /* test successful initialization of a reach */
    Reach reach = reach_new();

    for (int i = 0; i < n_nodes; i++)
        reach_put(reach, x[i], y[i], xs);

    reach_free(reach);
    xs_free(xs);
}

void
test_reach_node_props(void)
{
    int    i;
    int    n_nodes = 5;
    double x[]     = { 0, 1, 2, 3, 4 };
    double y[]     = { 0, 0.001, 0.002, 0.003, 0.004 };
    double q       = 0.1;
    double h       = 0.5;
    double wse;

    ReachNodeProps rnp;

    CrossSection xs = new_cross_section();

    Reach reach = reach_new();

    for (i = 0; i < n_nodes; i++) {
        reach_put(reach, x[i], y[i], xs);
    }

    for (i = 0; i < n_nodes; i++) {
        wse = h + y[i];
        rnp = reach_node_properties(reach, i, wse, q);
        rnp_free(rnp);
    }

    reach_free(reach);
    xs_free(xs);
}

void
test_reach_stream_distance(void)
{
    int    n_nodes = 5;
    double x[]     = { 0, 1, 2, 3, 4 };
    double y[]     = { 0, 0.001, 0.002, 0.003, 0.004 };

    double *stream_distance = calloc(n_nodes, sizeof(double));

    CrossSection xs = new_cross_section();

    Reach reach = reach_new();

    for (int i = 0; i < n_nodes; i++)
        reach_put(reach, x[i], y[i], xs);

    reach_stream_distance(reach, stream_distance);

    free(stream_distance);
    reach_free(reach);
    xs_free(xs);
}

void
test_reach(void)
{
    test_reach_new();
    test_reach_node_props();
    test_reach_stream_distance();
}
