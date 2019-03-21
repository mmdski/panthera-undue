#include "panthera.h"
#include "panthera/subsection.h"
#include "panthera/crosssection.h"
#include <math.h>

struct CrossSection {
    int n_coordinates;    /* number of coordinates */
    int n_subsections;    /* number of subsections */
    double ref_elevation; /* reference elevation */
    CoArray ca;           /* coordinate array */
    Subsection *ss;       /* array of subsections */
};

CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                    double *y_roughness) {

    if (!roughness || !y_roughness)
        RAISE(null_ptr_arg_Error);

    assert(n_roughness > 0);

    int i; /* loop variable */

    /* cross section to return */
    CrossSection xs;
    NEW(xs);
    xs->n_coordinates = coarray_length(ca);
    xs->n_subsections = n_roughness;
    xs->ref_elevation = coarray_min_z(ca);
    xs->ss            = Mem_calloc(n_roughness, sizeof(Subsection), __FILE__,
                                   __LINE__);

    /* CoArray with thalweg set to 0 elevation */
    CoArray normal_ca = coarray_subtract_z(ca, xs->ref_elevation);
    xs->ca            = normal_ca;

    /* initialize y splits
     * include first and last y-values of the CoArray
     */
    double y_splits[n_roughness + 1];
    y_splits[0] = coarray_get_y(normal_ca, 0);
    y_splits[n_roughness] = coarray_get_y(normal_ca,
                                          coarray_length(normal_ca) - 1);
    for (i = 1; i < n_roughness; i++) {
        y_splits[i] = *(y_roughness + i - 1);
    }

    /* set all activation depths to -inf */
    double activation_depth = -INFINITY;

    /* create subsections from the roughness section breaks */
    CoArray subarray;
    for (i = 0; i < n_roughness; i++) {
        subarray = coarray_subarray_y(normal_ca, y_splits[i], y_splits[i+1]);
        *(xs->ss + i) = ss_new(subarray, *(roughness + i), activation_depth);
        coarray_free(subarray);
    }

    return xs;
}

void xs_free(CrossSection xs) {
    int i;
    int n = xs->n_subsections;

    coarray_free(xs->ca);
    for (i = 0; i < n; i++) {
        ss_free(*(xs->ss + i));
    }
    Mem_free(xs->ss, __FILE__, __LINE__);
    FREE(xs);
}

HydraulicProps xs_hydraulic_properties(CrossSection xs, double wse) {

    int n_subsections = xs->n_subsections;
    int i;

    double area = 0;

    double depth = wse - xs->ref_elevation;

    HydraulicProps hp = hp_new();
    HydraulicProps *hp_ss = Mem_calloc(n_subsections, sizeof(HydraulicProps),
                                       __FILE__, __LINE__);

    for (i = 0; i < n_subsections; i++) {
        *(hp_ss + i) = ss_hydraulic_properties(*(xs->ss + i), depth);
        area += hp_get_property(*(hp_ss + i), HP_AREA);
        hp_free(*(hp_ss + i));
    }

    Mem_free(hp_ss, __FILE__, __LINE__);

    hp_set_property(hp, HP_AREA, area);

    return hp;
}

CoArray xs_coarray(CrossSection xs) {
    return coarray_subtract_z(xs->ca, -xs->ref_elevation);
}
