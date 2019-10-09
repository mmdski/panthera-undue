#include "mem.h"
#include "secantsolve.h"
#include "subsection.h"
#include <assert.h>
#include <math.h>
#include <panthera/constants.h>
#include <panthera/crosssection.h>
#include <panthera/xsproperties.h>
#include <stdlib.h>

/*
 * cross section interface
 */
struct CrossSection {
    int         n_coordinates; /* number of coordinates */
    int         n_subsections; /* number of subsections */
    double      min_y;         /* minimum y value (thalweg) */
    CoArray     ca;            /* coordinate array */
    Subsection *ss;            /* array of subsections */
};

static CrossSectionProps
calc_hydraulic_properties(CrossSection xs, double h)
{

    assert(xs);

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

    CrossSectionProps xsp = xsp_new();
    CrossSectionProps xsp_ss;
    Subsection        ss;

    for (i = 0; i < n_subsections; i++) {

        /* skip subsection if depth is less than the lowest point in the
         * subsection */
        ss = *(xs->ss + i);
        if (subsection_activated(ss, h))
            continue;

        xsp_ss  = subsection_properties(ss, h);
        area_ss = xsp_get(xsp_ss, XS_AREA);
        k_ss    = xsp_get(xsp_ss, XS_CONVEYANCE);
        top_width += xsp_get(xsp_ss, XS_TOP_WIDTH);
        w_perimeter += xsp_get(xsp_ss, XS_WETTED_PERIMETER);

        if (area_ss > 0) {
            sum += (k_ss * k_ss * k_ss) / (area_ss * area_ss);
        }

        xsp_free(xsp_ss);

        area += area_ss;
        conveyance += k_ss;
    }

    h_depth  = area / top_width;
    h_radius = area / w_perimeter;
    if (isnan(h_radius))
        conveyance = NAN;
    alpha     = (area * area) * sum / (conveyance * conveyance * conveyance);
    crit_flow = area * sqrt(GRAVITY * h_depth);

    xsp_set(xsp, XS_DEPTH, h);
    xsp_set(xsp, XS_AREA, area);
    xsp_set(xsp, XS_TOP_WIDTH, top_width);
    xsp_set(xsp, XS_WETTED_PERIMETER, w_perimeter);
    xsp_set(xsp, XS_HYDRAULIC_DEPTH, h_depth);
    xsp_set(xsp, XS_HYDRAULIC_RADIUS, h_radius);
    xsp_set(xsp, XS_CONVEYANCE, conveyance);
    xsp_set(xsp, XS_VELOCITY_COEFF, alpha);
    xsp_set(xsp, XS_CRITICAL_FLOW, crit_flow);

    return xsp;
}

CrossSection
xs_new(CoArray ca, int n_roughness, double *roughness, double *z_roughness)
{
    assert(n_roughness >= 1);
    assert(roughness);
    if (n_roughness > 1)
        assert(z_roughness);
    for (int i = 0; i < n_roughness; i++)
        assert(roughness > 0);

    Coordinate c;

    /* cross section to return */
    CrossSection xs;
    NEW(xs);
    xs->n_coordinates = coarray_length(ca);
    xs->n_subsections = n_roughness;
    xs->ss = mem_calloc(n_roughness, sizeof(Subsection), __FILE__, __LINE__);
    xs->ca = coarray_copy(ca);

    /* initialize z splits
     * include first and last z-values of the CoArray
     */
    double *z_splits =
        mem_calloc(n_roughness + 1, sizeof(double), __FILE__, __LINE__);

    c           = coarray_get(xs->ca, 0);
    z_splits[0] = c->z;
    coord_free(c);

    c                     = coarray_get(xs->ca, coarray_length(xs->ca) - 1);
    z_splits[n_roughness] = c->z;
    coord_free(c);

    for (int i = 1; i < n_roughness; i++) {
        z_splits[i] = *(z_roughness + i - 1);
    }

    /* set all activation depths to -inf */
    double activation_depth = -INFINITY;

    /* create subsections from the roughness section breaks */
    CoArray subarray;
    for (int i = 0; i < n_roughness; i++) {
        subarray = coarray_subarray(xs->ca, z_splits[i], z_splits[i + 1]);
        *(xs->ss + i) =
            subsection_new(subarray, *(roughness + i), activation_depth);
        coarray_free(subarray);
    }

    mem_free(z_splits, __FILE__, __LINE__);

    return xs;
}

void
xs_free(CrossSection xs)
{

    assert(xs);

    int i;
    int n = xs->n_subsections;

    /* free the coordinate array */
    coarray_free(xs->ca);

    /* free the subsections and subsection array */
    for (i = 0; i < n; i++) {
        subsection_free(*(xs->ss + i));
    }
    mem_free(xs->ss, __FILE__, __LINE__);

    FREE(xs);
}

CrossSectionProps
xs_hydraulic_properties(CrossSection xs, double y)
{
    assert(xs);

    if (!isfinite(y))
        return NULL;

    return calc_hydraulic_properties(xs, y);
}

CoArray
xs_coarray(CrossSection xs)
{
    assert(xs);

    return coarray_copy(xs->ca);
}

int
xs_n_subsections(CrossSection xs)
{
    assert(xs);

    return xs->n_subsections;
}

void
xs_roughness(CrossSection xs, double *roughness)
{
    assert(xs && roughness);

    int        i;
    int        n_subsections = xs->n_subsections;
    Subsection ss;

    for (i = 0; i < n_subsections; i++) {
        ss               = *(xs->ss + i);
        *(roughness + i) = subsection_roughness(ss);
    }
}

void
xs_z_roughness(CrossSection xs, double *z_roughness)
{
    assert(xs && z_roughness);

    int        i;
    int        n_subsections = xs->n_subsections;
    Subsection ss;

    for (i = 0; i < n_subsections - 1; i++) {
        ss                 = *(xs->ss + i);
        *(z_roughness + i) = subsection_z(ss);
    }
}

/* critical depth solver */
typedef struct {
    double       discharge;
    CrossSection xs;
} CriticalDepthData;

double
critical_flow_zero(double h, void *function_data)
{
    double             critical_flow;
    CrossSectionProps  xsp;
    CriticalDepthData *solver_data = (CriticalDepthData *) function_data;

    if (!isfinite(h))
        return NAN;

    xsp           = xs_hydraulic_properties(solver_data->xs, h);
    critical_flow = xsp_get(xsp, XS_CRITICAL_FLOW);
    xsp_free(xsp);

    return critical_flow - solver_data->discharge;
}

static double
calc_critical_depth(CrossSection xs, double discharge, double initial_h)
{
    assert(xs);

    int             max_iterations = 20;
    double          eps            = 0.003;
    double          critical_depth = NAN;
    double          err;
    double          h_1;
    SecantSolution *res;

    CriticalDepthData func_data = { discharge, xs };
    err = critical_flow_zero(initial_h, (void *) &func_data);
    h_1 = initial_h + 0.7 * err;

    res = secant_solve(
        max_iterations, eps, &critical_flow_zero, &func_data, initial_h, h_1);
    if (res->solution_found)
        critical_depth = res->x_computed;
    FREE(res);

    return critical_depth;
}

double
xs_critical_depth(CrossSection xs, double discharge, double initial_depth)
{
    assert(xs);

    double critical_depth = calc_critical_depth(xs, discharge, initial_depth);

    return critical_depth;
}

/* normal depth solver */
typedef struct {
    double       discharge;
    double       sqrt_slope;
    CrossSection xs;
} NormalDepthData;

double
normal_flow_zero(double h, void *function_data)
{
    double            conveyance;
    CrossSectionProps xsp;
    NormalDepthData * solver_data = (NormalDepthData *) function_data;

    xsp        = xs_hydraulic_properties(solver_data->xs, h);
    conveyance = xsp_get(xsp, XS_CONVEYANCE);
    xsp_free(xsp);

    return conveyance * solver_data->sqrt_slope - solver_data->discharge;
}

static double
calc_normal_depth(CrossSection xs,
                  double       discharge,
                  double       slope,
                  double       initial_h)
{
    assert(xs);
    int             max_iterations = 20;
    double          eps            = 0.003;
    double          normal_depth   = NAN;
    double          err;
    double          h_1;
    SecantSolution *res;

    NormalDepthData func_data = { discharge, sqrt(slope), xs };
    err = normal_flow_zero(initial_h, (void *) &func_data);
    h_1 = initial_h + 0.7 * err;

    res = secant_solve(
        max_iterations, eps, &normal_flow_zero, &func_data, initial_h, h_1);
    if (res->solution_found)
        normal_depth = res->x_computed;
    FREE(res);

    return normal_depth;
}

double
xs_normal_depth(CrossSection xs,
                double       discharge,
                double       slope,
                double       initial_depth)
{
    assert(xs);

    double normal_depth =
        calc_normal_depth(xs, discharge, slope, initial_depth);

    return normal_depth;
}
