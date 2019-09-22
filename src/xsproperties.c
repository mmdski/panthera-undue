#include "xsproperties.h"
#include "mem.h"
#include <assert.h>

/* cross section properties interface */

struct CrossSectionProps {
    double *properties;
};

CrossSectionProps
xsp_new(void)
{
    CrossSectionProps xsp;
    NEW(xsp);
    xsp->properties = mem_calloc(N_XSP, sizeof(double), __FILE__, __LINE__);
    return xsp;
}

void
xsp_free(CrossSectionProps xsp)
{
    assert(xsp);
    mem_free(xsp->properties, __FILE__, __LINE__);
    FREE(xsp);
}

CrossSectionProps
xsp_copy(CrossSectionProps xsp)
{
    assert(xsp);
    double            value;
    CrossSectionProps new_xsp = xsp_new();
    for (int i = 0; i < N_XSP; i++) {
        value = xsp_get(xsp, i);
        xsp_set(new_xsp, i, value);
    }
    return new_xsp;
}

double
xsp_get(CrossSectionProps xsp, xs_prop prop)
{
    assert(xsp);
    return *(xsp->properties + prop);
}

void
xsp_set(CrossSectionProps xsp, xs_prop prop, double value)
{
    assert(xsp);
    *(xsp->properties + prop) = value;
}

CrossSectionProps
xsp_interp_depth(CrossSectionProps xsp1, CrossSectionProps xsp2, double depth)
{
    double d1 = *(xsp1->properties + XS_DEPTH);
    double d2 = *(xsp2->properties + XS_DEPTH);

    assert(d1 <= depth && depth <= d2);

    CrossSectionProps xsp = xsp_new();

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
