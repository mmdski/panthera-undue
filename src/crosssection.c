#include "panthera.h"
#include "panthera/subsection.h"
#include "panthera/crosssection.h"
#include <math.h>

struct CrossSection {
    int n_subsections;    /* number of subsections */
    double ref_elevation; /* reference elevation */
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
    xs->ss = Mem_calloc(n_roughness, sizeof(Subsection), __FILE__, __LINE__);
    xs->ref_elevation = coarray_min_z(ca);
    xs->n_subsections = n_roughness;

    /* CoArray with thalweg set to 0 elevation */
    CoArray normal_ca = coarray_subtract_z(ca, xs->ref_elevation);

    /* initialize y splits
     * include first and last y-values of the CoArray
     */
    double y_splits[n_roughness + 1];
    y_splits[0] = coarray_get_y(normal_ca, 0);
    y_splits[n_roughness] = coarray_get_y(normal_ca,
                                          coarray_length(normal_ca) - 1);
    for (i = 1; i < n_roughness; i++) {
        y_splits[i] = *(y_roughness + i);
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

    coarray_free(normal_ca);

    return xs;
}

void xs_free(CrossSection xs) {
    int i;
    int n = xs->n_subsections;

    for (i = 0; i < n; i++) {
        ss_free(*(xs->ss + i));
    }
    Mem_free(xs->ss, __FILE__, __LINE__);
    FREE(xs);
}
