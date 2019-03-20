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

    /* cross section to return */
    CrossSection xs;
    NEW(xs);
    xs->ss = Mem_calloc(n_roughness, sizeof(Subsection), __FILE__, __LINE__);
    xs->ref_elevation = coarray_min_z(ca);
    xs->n_subsections = n_roughness;

    /* initialize y range to include -INFINITY to the first y value*/
    double ylo = -INFINITY;
    double yhi = *y_roughness;

    double activation_depth = -INFINITY;

    CoArray subarray = coarray_subarray_y(ca, ylo, yhi);
    *(xs->ss) = ss_new(subarray, *roughness, activation_depth);

    int i; /* array variable */

    for (i = 1; i < n_roughness; i++) {

        coarray_free(subarray);
        ylo = *(y_roughness + i - 1);
        yhi = *(y_roughness + i);
        subarray = coarray_subarray_y(ca, ylo, yhi);
        *(xs->ss + i) = ss_new(subarray, *(roughness + i), activation_depth);
    }
    coarray_free(subarray);

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
