#include <math.h>
#include <panthera/cii/mem.h>
#include <panthera/constants.h>
#include <panthera/crosssection.h>
#include <panthera/exceptions.h>
#include <stddef.h>

/* cross section properties interface */

struct CrossSectionProps {
    double *properties;
};

static CrossSectionProps
xsp_new (void)
{
    CrossSectionProps xsp;
    NEW (xsp);
    xsp->properties = Mem_calloc (N_XSP, sizeof (double), __FILE__, __LINE__);
    return xsp;
}

void
xsp_free (CrossSectionProps xsp)
{
    if (!xsp)
        RAISE (null_ptr_arg_error);
    Mem_free (xsp->properties, __FILE__, __LINE__);
    FREE (xsp);
}

double
xsp_get (CrossSectionProps xsp, xs_prop prop)
{
    if (!xsp)
        RAISE (null_ptr_arg_error);
    return *(xsp->properties + prop);
}

static void
xsp_set (CrossSectionProps xsp, xs_prop prop, double value)
{
    if (!xsp)
        RAISE (null_ptr_arg_error);
    *(xsp->properties + prop) = value;
}

static CrossSectionProps
xsp_interp_depth (CrossSectionProps xsp1, CrossSectionProps xsp2, double depth)
{
    double d1 = *(xsp1->properties + XS_DEPTH);
    double d2 = *(xsp2->properties + XS_DEPTH);

    assert (d1 <= depth && depth <= d2);

    CrossSectionProps xsp = xsp_new ();

    double prop1;
    double prop2;

    int i;

    for (i = 0; i < N_XSP; i++) {
        prop1 = *(xsp1->properties + i);
        prop2 = *(xsp2->properties + i);
        *(xsp->properties + i) =
            (prop2 - prop1) / (d2 - d1) * (depth - d1) + prop1;
    }

    return xsp;
}

/* subsection interface */

struct Subsection {
    CoArray array;     /* coordinate array */
    double  n;         /* Manning's n */
    double  min_depth; /* activation depth */
};

typedef struct Subsection *Subsection;

/* Allocates memory and creates a new Subsection */
static Subsection
ss_new (CoArray ca, double roughness, double activation_depth)
{
    assert ((int) (roughness > 0));

    Subsection ss;
    NEW (ss);

    ss->array     = coarray_copy (ca);
    ss->n         = roughness;
    ss->min_depth = activation_depth;

    return ss;
}

/* Frees memory from a previously allocated Subsection */
static void
ss_free (Subsection ss)
{
    coarray_free (ss->array);
    FREE (ss);
}

/* Calculates hydraulic properties for the subsection.
 * Returns a new CrossSectionProps.
 */
static CrossSectionProps
ss_hydraulic_properties (Subsection ss, double y)
{
    assert (ss);

    CoArray sa;

    double area      = 0;
    double perimeter = 0;
    double top_width = 0;
    double hydraulic_radius;
    double conveyance;

    CrossSectionProps xsp = xsp_new ();

    int n;

    /* return 0 subsection values if this subsection isn't activated */
    if (y < coarray_min_y (ss->array) || y <= ss->min_depth) {
        sa = NULL;
        n  = 0;
    }
    /* otherwise calculate the values */
    else {
        sa = coarray_subarray_y (ss->array, y);
        n  = coarray_length (sa);
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
        y1 = coarray_get_y (sa, i - 1);
        z1 = coarray_get_z (sa, i - 1);

        y2 = coarray_get_y (sa, i);
        z2 = coarray_get_z (sa, i);

        /* if y1 or y2 is NAN, continue */
        if (isnan (y1) || isnan (y2)) {
            continue;
        }

        /* calculate area by trapezoidal integration */
        d1 = y - y1;
        d2 = y - y2;
        area += 0.5 * (d1 + d2) * (z2 - z1);

        /* calculate perimeter */
        dy = y2 - y1;
        dz = z2 - z1;
        perimeter += sqrt (dy * dy + dz * dz);

        /* calculate top width */
        top_width += z2 - z1;
    }

    hydraulic_radius = area / perimeter;
    conveyance       = 1 / ss->n * area * pow (hydraulic_radius, 2.0 / 3.0);

    xsp_set (xsp, XS_AREA, area);
    xsp_set (xsp, XS_TOP_WIDTH, top_width);
    xsp_set (xsp, XS_WETTED_PERIMETER, perimeter);
    xsp_set (xsp, XS_HYDRAULIC_RADIUS, hydraulic_radius);
    xsp_set (xsp, XS_CONVEYANCE, conveyance);

    if (sa)
        coarray_free (sa);

    return xsp;
}

static double
ss_roughness (Subsection ss)
{
    assert (ss);
    return ss->n;
}

/*
 * results cache interface
 */
typedef struct ResultsCache {
    int                size;
    CrossSectionProps *xsp;
} ResultsCache;

#define DEPTH_INTERP_DELTA 0.1  /* depth interpolation step size */
#define CACHE_EXPANSION_TERM 10 /* rate to grow array */

#define calc_index(depth) (int) (depth / DEPTH_INTERP_DELTA)
#define calc_depth(index) DEPTH_INTERP_DELTA *index

static ResultsCache *
res_new (int size)
{
    assert (size > 0);

    ResultsCache *res;
    NEW (res);

    /* allocate space for CrossSectionProps pointers */
    CrossSectionProps *xsp =
        Mem_calloc (size, sizeof (CrossSectionProps), __FILE__, __LINE__);

    /* set results to NULL */
    int i;
    for (i = 0; i < size; i++) {
        *(xsp + i) = NULL;
    }

    res->size = size;
    res->xsp  = xsp;

    return res;
}

static void
res_free (ResultsCache *res)
{
    assert (res);

    int               i;
    int               size = res->size;
    CrossSectionProps xsp;
    for (i = 0; i < size; i++) {
        if ((xsp = *(res->xsp + i))) {
            xsp_free (xsp);
        }
    }
    Mem_free (res->xsp, __FILE__, __LINE__);
    FREE (res);
}

static ResultsCache *
res_resize (int new_size, ResultsCache *old_res)
{
    assert (old_res);
    assert (new_size > old_res->size);

    int i;
    int n = old_res->size;

    ResultsCache *new_res = res_new (new_size);

    for (i = 0; i < n; i++)
        *(new_res->xsp + i) = *(old_res->xsp + i);

    Mem_free (old_res->xsp, __FILE__, __LINE__);
    FREE (old_res);

    return new_res;
}

/*
 * cross section interface
 */

struct CrossSection {
    int           n_coordinates; /* number of coordinates */
    int           n_subsections; /* number of subsections */
    CoArray       ca;            /* coordinate array */
    Subsection *  ss;            /* array of subsections */
    ResultsCache *results;       /* results cache */
};

static CrossSectionProps
_calc_hydraulic_properties (CrossSection xs, double h)
{

    assert (xs);

    int n_subsections = xs->n_subsections;
    int i;

    double area        = 0; /* area */
    double area_ss     = 0; /* subsection area */
    double top_width   = 0; /* top width */
    double w_perimeter = 0; /* wetted perimeter */
    double h_depth;         /* hydraulic depth */
    double h_radius;        /* hydraulic radius */
    double conveyance = 0;  /* conveyance */
    double k_ss       = 0;  /* subsection conveyance */
    double sum        = 0;  /* sum for velocity coefficient */
    double alpha;           /* velocity coefficient */
    double crit_flow;       /* critical flow */

    CrossSectionProps xsp = xsp_new ();
    CrossSectionProps xsp_ss;
    Subsection        ss;

    for (i = 0; i < n_subsections; i++) {
        ss      = *(xs->ss + i);
        xsp_ss  = ss_hydraulic_properties (ss, h);
        area_ss = xsp_get (xsp_ss, XS_AREA);
        k_ss    = xsp_get (xsp_ss, XS_CONVEYANCE);
        top_width += xsp_get (xsp_ss, XS_TOP_WIDTH);
        w_perimeter += xsp_get (xsp_ss, XS_WETTED_PERIMETER);

        if (area_ss > 0) {
            sum += (k_ss * k_ss * k_ss) / (area_ss * area_ss);
        }

        xsp_free (xsp_ss);

        area += area_ss;
        conveyance += k_ss;
    }

    h_depth   = area / top_width;
    h_radius  = area / w_perimeter;
    alpha     = (area * area) * sum / (conveyance * conveyance * conveyance);
    crit_flow = area * sqrt (GRAVITY * area / (alpha * top_width));

    xsp_set (xsp, XS_DEPTH, h);
    xsp_set (xsp, XS_AREA, area);
    xsp_set (xsp, XS_TOP_WIDTH, top_width);
    xsp_set (xsp, XS_WETTED_PERIMETER, w_perimeter);
    xsp_set (xsp, XS_HYDRAULIC_DEPTH, h_depth);
    xsp_set (xsp, XS_HYDRAULIC_RADIUS, h_radius);
    xsp_set (xsp, XS_CONVEYANCE, conveyance);
    xsp_set (xsp, XS_VELOCITY_COEFF, alpha);
    xsp_set (xsp, XS_CRITICAL_FLOW, crit_flow);

    return xsp;
}

/* interfacing function between results cache and cross section */
static CrossSectionProps
xs_get_properties_from_res (CrossSection xs, double h)
{

    assert (xs);
    assert (!isnan (h));

    int indlo = calc_index (h);
    int indhi = indlo + 1;

    if (indhi > (xs->results->size - 1)) {
        int new_size = indhi + CACHE_EXPANSION_TERM;
        xs->results  = res_resize (new_size, xs->results);
    }

    double dlo;
    double dhi;

    CrossSectionProps xsplo = *(xs->results->xsp + indlo);
    if (!xsplo) {
        dlo                         = calc_depth (indlo);
        xsplo                       = _calc_hydraulic_properties (xs, dlo);
        *(xs->results->xsp + indlo) = xsplo;
    }

    CrossSectionProps xsphi = *(xs->results->xsp + indhi);
    if (!xsphi) {
        dhi                         = calc_depth (indhi);
        xsphi                       = _calc_hydraulic_properties (xs, dhi);
        *(xs->results->xsp + indhi) = xsphi;
    }

    CrossSectionProps xsp = xsp_interp_depth (xsplo, xsphi, h);

    return xsp;
}

CrossSection
xs_new (CoArray ca, int n_roughness, double *roughness, double *z_roughness)
{

    if (n_roughness < 1)
        RAISE (value_arg_error);

    if (!roughness)
        RAISE (null_ptr_arg_error);

    if (n_roughness > 1 && !z_roughness)
        RAISE (null_ptr_arg_error);

    int i; /* loop variable */

    for (i = 0; i < n_roughness; i++)
        if (*(roughness + i) <= 0)
            RAISE (value_arg_error);

    /* cross section to return */
    CrossSection xs;
    NEW (xs);
    xs->n_coordinates = coarray_length (ca);
    xs->n_subsections = n_roughness;
    xs->ss = Mem_calloc (n_roughness, sizeof (Subsection), __FILE__, __LINE__);

    xs->ca = coarray_copy (ca);

    /* initialize a results cache to store depths up to 2 *
     * DEPTH_INTERP_DELTA
     */
    xs->results = res_new (1);

    /* initialize z splits
     * include first and last z-values of the CoArray
     */
    double *z_splits =
        Mem_calloc (n_roughness + 1, sizeof (double), __FILE__, __LINE__);
    z_splits[0] = coarray_get_z (xs->ca, 0);
    z_splits[n_roughness] =
        coarray_get_z (xs->ca, coarray_length (xs->ca) - 1);
    for (i = 1; i < n_roughness; i++) {
        z_splits[i] = *(z_roughness + i - 1);
    }

    /* set all activation depths to -inf */
    double activation_depth = -INFINITY;

    /* create subsections from the roughness section breaks */
    CoArray subarray;
    for (i = 0; i < n_roughness; i++) {
        subarray = coarray_subarray_z (xs->ca, z_splits[i], z_splits[i + 1]);
        *(xs->ss + i) = ss_new (subarray, *(roughness + i), activation_depth);
        coarray_free (subarray);
    }

    Mem_free (z_splits, __FILE__, __LINE__);

    return xs;
}

void
xs_free (CrossSection xs)
{

    if (!xs)
        RAISE (null_ptr_arg_error);

    int i;
    int n = xs->n_subsections;

    /* free the coordinate array */
    coarray_free (xs->ca);

    /* free the subsections and subsection array */
    for (i = 0; i < n; i++) {
        ss_free (*(xs->ss + i));
    }
    Mem_free (xs->ss, __FILE__, __LINE__);

    /* free the results cache */
    res_free (xs->results);
    FREE (xs);
}

CrossSectionProps
xs_hydraulic_properties (CrossSection xs, double h)
{
    if (!xs)
        RAISE (null_ptr_arg_error);

    if (h < coarray_min_y (xs->ca))
        RAISE (xsp_depth_error);

    if (isnan (h) || isinf (h))
        RAISE (xs_invld_depth_error);

    return xs_get_properties_from_res (xs, h);
}

CoArray
xs_coarray (CrossSection xs)
{
    if (!xs)
        RAISE (null_ptr_arg_error);

    return coarray_copy (xs->ca);
}

int
xs_n_subsections (CrossSection xs)
{
    if (!xs)
        RAISE (null_ptr_arg_error);

    return xs->n_subsections;
}

void
xs_roughness (CrossSection xs, double *roughness)
{
    if (!xs || !roughness)
        RAISE (null_ptr_arg_error);

    int        i;
    int        n_subsections = xs->n_subsections;
    Subsection ss;

    for (i = 0; i < n_subsections; i++) {
        ss               = *(xs->ss + i);
        *(roughness + i) = ss_roughness (ss);
    }
}

void
xs_z_roughness (CrossSection xs, double *z_roughness)
{
    if (!xs)
        RAISE (null_ptr_arg_error);

    int        i;
    int        n_subsections = xs->n_subsections;
    Subsection ss;

    for (i = 1; i < n_subsections; i++) {
        ss                     = *(xs->ss + i);
        *(z_roughness + i - 1) = coarray_get_z (ss->array, 0);
    }
}
