cdef extern from "panthera/xsproperties.h":

    ctypedef struct CrossSectionProps:
        pass

    ctypedef enum xs_prop:
        XS_DEPTH,
        XS_AREA,
        XS_TOP_WIDTH,
        XS_WETTED_PERIMETER,
        XS_HYDRAULIC_DEPTH,
        XS_HYDRAULIC_RADIUS,
        XS_CONVEYANCE,
        XS_VELOCITY_COEFF,
        XS_CRITICAL_FLOW,
        N_XSP

    cdef void xsp_free(CrossSectionProps xsp)
    cdef double xsp_get(CrossSectionProps xsp, xs_prop prop)
