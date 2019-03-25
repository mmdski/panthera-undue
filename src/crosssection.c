#include <panthera/crosssection.h>

/* hydraulic properties interface */

struct HydraulicProps {
    double *properties;
};

HydraulicProps hp_new(void) {
    HydraulicProps hp;
    NEW(hp);
    hp->properties = Mem_calloc(N_HP, sizeof(double), __FILE__, __LINE__);
    return hp;
}

void hp_free(HydraulicProps hp) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    Mem_free(hp->properties, __FILE__, __LINE__);
    FREE(hp);
}

double hp_get(HydraulicProps hp, hyd_prop prop) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    return *(hp->properties + prop);
}

void hp_set(HydraulicProps hp, hyd_prop prop, double value) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    *(hp->properties + prop) = value;
}

HydraulicProps hp_interp_depth(HydraulicProps hp1, HydraulicProps hp2,
                               double depth) {
    double d1 = *(hp1->properties + HP_DEPTH);
    double d2 = *(hp2->properties + HP_DEPTH);

    assert(d1 <= depth && depth <= d2);

    HydraulicProps hp = hp_new();

    double prop1;
    double prop2;

    int i;

    for (i = 0; i < N_HP; i++) {
        prop1 = *(hp1->properties + i);
        prop2 = *(hp2->properties + i);
        *(hp->properties + i) = (prop2 - prop1) / (d2 - d1) * (depth - d1) +
            prop1;
    }

    return hp;
}

/* subsection interface */

struct Subsection {
    CoArray array;    /* coordinate array */
    double n;         /* Manning's n */
    double min_depth; /* activation depth */
};

typedef struct Subsection *Subsection;

/* Allocates memory and creates a new Subsection */
Subsection ss_new(CoArray ca, double roughness, double activation_depth) {

    assert((int)(roughness > 0));

    Subsection ss;
    NEW(ss);

    ss->array     = coarray_copy(ca);
    ss->n         = roughness;
    ss->min_depth = activation_depth;

    return ss;
}

/* Frees memory from a previously allocated Subsection */
void ss_free(Subsection ss) {
    coarray_free(ss->array);
    FREE(ss);
}

/* Calculates hydraulic properties for the subsection.
 * Returns a new HydraulicProps.
 */
HydraulicProps ss_hydraulic_properties(Subsection ss, double z) {

    assert(ss);

    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;

    HydraulicProps hp = hp_new();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (z < coarray_min_z(ss->array) || z <= ss->min_depth) {
        sa = NULL;
        n  = 0;
    }
    /* otherwise calculate the values */
    else {
        sa = coarray_subarray_z(ss->array, z);
        n  = coarray_length(sa);
    }

    int i;

    /* depth for c1 and c2 */
    double d1;
    double d2;

    /* distances for perimeter */
    double dy;
    double dz;

    double y1;
    double z1;

    double y2;
    double z2;

    for (i = 1; i < n; i++) {
        y1 = coarray_get_y(sa, i - 1);
        z1 = coarray_get_z(sa, i - 1);

        y2 = coarray_get_y(sa, i);
        z2 = coarray_get_z(sa, i);

        /* if y1 or y2 is NAN, continue */
        if (isnan(y1) || isnan(y2)) {
            continue;
        }

        /* calculate area by trapezoidal integration */
        d1 = z - z1;
        d2 = z - z2;
        area += 0.5 * (d1 + d2) * (y2 - y1);

        /* calculate perimeter */
        dy = y2 - y1;
        dz = z2 - z1;
        perimeter += sqrt(dy * dy + dz * dz);

        /* calculate top width */
        top_width += y2 - y1;
    }

    hp_set(hp, HP_AREA, area);
    hp_set(hp, HP_TOP_WIDTH, top_width);
    hp_set(hp, HP_WETTED_PERIMETER, perimeter);

    if (sa)
        coarray_free(sa);

    return hp;
}

/* results cache interface */
typedef struct ResultsCache {
    int size;
    double max_depth;
    HydraulicProps *hp;
} ResultsCache;

#define DEPTH_INTERP_DELTA 0.1  /* depth interpolation step size */
#define CACHE_RESIZE_RATE 1.5   /* rate to grow array */

#define calc_index(depth) (int) (depth / DEPTH_INTERP_DELTA)
#define calc_depth(index) DEPTH_INTERP_DELTA * index

ResultsCache *res_new(double max_depth) {

    assert(max_depth > 0);

    ResultsCache *res;
    NEW(res);

    int size = calc_index(max_depth) + 1;

    /* allocate space for HydraulicProps pointers */
    HydraulicProps *hp = Mem_calloc(size, sizeof(HydraulicProps),
                                         __FILE__, __LINE__);

    /* set results to NULL */
    int i;
    for (i = 0; i < size; i++) {
        *(hp + i) = NULL;
    }

    res->size      = size;
    res->max_depth = max_depth;
    res->hp   = hp;

    return res;
}

void res_free(ResultsCache *res) {
    assert(res);

    int i;
    int size = res->size;
    HydraulicProps hp;
    for (i = 0; i < size; i++) {
        if ((hp = *(res->hp + i))) {
            hp_free(hp);
        }
    }
    Mem_free(res->hp, __FILE__, __LINE__);
    FREE(res);
}

ResultsCache *res_resize(double max_depth, ResultsCache *old_res) {
    assert(old_res);

    int i;
    int n = old_res->size;

    ResultsCache *new_res = res_new(max_depth);

    for (i = 0; i < n; i++)
        *(new_res->hp + i) = *(old_res->hp + i);

    Mem_free(old_res->hp, __FILE__, __LINE__);
    FREE(old_res);

    return new_res;
}

/* cross section interface */

struct CrossSection {
    int n_coordinates;     /* number of coordinates */
    int n_subsections;     /* number of subsections */
    double ref_elevation;  /* reference elevation */
    CoArray ca;            /* coordinate array */
    Subsection *ss;        /* array of subsections */
    ResultsCache *results; /* results cache */
};

HydraulicProps _calc_hydraulic_properties(CrossSection xs, double depth) {

    assert(xs);

    int n_subsections = xs->n_subsections;
    int i;

    double area             = 0;
    double top_width        = 0;
    double perimeter        = 0;
    double hydraulic_depth;
    double hydraulic_radius;

    HydraulicProps hp = hp_new();
    HydraulicProps hp_ss;

    for (i = 0; i < n_subsections; i++) {
        hp_ss      = ss_hydraulic_properties(*(xs->ss + i), depth);
        area      += hp_get(hp_ss, HP_AREA);
        top_width += hp_get(hp_ss, HP_TOP_WIDTH);
        perimeter += hp_get(hp_ss, HP_WETTED_PERIMETER);
        hp_free(hp_ss);
    }

    /* if area is zero, assume top_width and perimeter are also 0 and set
     * hydraulic_depth and hydraulic_radius to 0.
     */
    if (area == 0) {
        hydraulic_depth = 0;
        hydraulic_radius = 0;
    } else {
        hydraulic_depth  = area / top_width;
        hydraulic_radius = area / perimeter;
    }

    hp_set(hp, HP_DEPTH, depth);
    hp_set(hp, HP_AREA, area);
    hp_set(hp, HP_TOP_WIDTH, top_width);
    hp_set(hp, HP_WETTED_PERIMETER, perimeter);
    hp_set(hp, HP_HYDRAULIC_DEPTH, hydraulic_depth);
    hp_set(hp, HP_HYDRAULIC_RADIUS, hydraulic_radius);

    return hp;
}

/* interfacing function between results cache and cross section */
HydraulicProps xs_get_properties_from_res(CrossSection xs, double depth) {

    assert(xs);

    if (depth > xs->results->max_depth - DEPTH_INTERP_DELTA) {
        xs->results = res_resize(CACHE_RESIZE_RATE * depth, xs->results);
    }

    int indlo = calc_index(depth);
    int indhi = indlo + 1;

    double dlo;
    double dhi;

    HydraulicProps hplo = *(xs->results->hp + indlo);
    if (!hplo) {
        dlo  = calc_depth(indlo);
        hplo = _calc_hydraulic_properties(xs, dlo);
        *(xs->results->hp + indlo) = hplo;
    }

    HydraulicProps hphi = *(xs->results->hp + indhi);
    if (!hphi) {
        dhi  = calc_depth(indhi);
        hphi = _calc_hydraulic_properties(xs, dhi);
        *(xs->results->hp + indhi) = hphi;
    }

    HydraulicProps hp = hp_interp_depth(hplo, hphi, depth);

    return hp;
}

CrossSection xs_new(CoArray ca, int n_roughness, double *roughness,
                    double *y_roughness) {

    if (n_roughness < 1)
        RAISE(value_arg_Error);

    if (!roughness)
        RAISE(null_ptr_arg_Error);

    if (n_roughness > 1 && !y_roughness)
        RAISE(null_ptr_arg_Error);

    int i; /* loop variable */

    for (i = 0; i < n_roughness; i++)
        if (*(roughness + i) <= 0)
            RAISE(value_arg_Error);

    /* cross section to return */
    CrossSection xs;
    NEW(xs);
    xs->n_coordinates = coarray_length(ca);
    xs->n_subsections = n_roughness;
    xs->ref_elevation = coarray_min_z(ca);
    xs->ss = Mem_calloc(n_roughness, sizeof(Subsection), __FILE__, __LINE__);

    /* CoArray with thalweg set to 0 elevation */
    CoArray normal_ca = coarray_add_z(ca, -xs->ref_elevation);
    xs->ca            = normal_ca;

    /* initialize a results cache to store depths up to 2 * DEPTH_INTERP_DELTA
     */
    xs->results = res_new(2 * DEPTH_INTERP_DELTA);

    /* initialize y splits
     * include first and last y-values of the CoArray
     */
    double y_splits[n_roughness + 1];
    y_splits[0] = coarray_get_y(normal_ca, 0);
    y_splits[n_roughness] =
        coarray_get_y(normal_ca, coarray_length(normal_ca) - 1);
    for (i = 1; i < n_roughness; i++) {
        y_splits[i] = *(y_roughness + i - 1);
    }

    /* set all activation depths to -inf */
    double activation_depth = -INFINITY;

    /* create subsections from the roughness section breaks */
    CoArray subarray;
    for (i = 0; i < n_roughness; i++) {
        subarray = coarray_subarray_y(normal_ca, y_splits[i], y_splits[i + 1]);
        *(xs->ss + i) = ss_new(subarray, *(roughness + i), activation_depth);
        coarray_free(subarray);
    }

    return xs;
}

void xs_free(CrossSection xs) {

    if (!xs)
        RAISE(null_ptr_arg_Error);

    int i;
    int n = xs->n_subsections;

    /* free the coordinate array */
    coarray_free(xs->ca);

    /* free the subsections and subsection array */
    for (i = 0; i < n; i++) {
        ss_free(*(xs->ss + i));
    }
    Mem_free(xs->ss, __FILE__, __LINE__);

    /* free the results cache */
    res_free(xs->results);
    FREE(xs);
}

HydraulicProps xs_hydraulic_properties(CrossSection xs, double wse) {
    if (!xs)
        RAISE(null_ptr_arg_Error);

    double depth = wse - xs->ref_elevation;

    return xs_get_properties_from_res(xs, depth);
}

CoArray xs_coarray(CrossSection xs) {
    if (!xs)
        RAISE(null_ptr_arg_Error);

    return coarray_add_z(xs->ca, xs->ref_elevation);
}
