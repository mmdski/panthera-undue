cdef extern from "panthera/crosssection.h":

    # coordinate

    cdef struct Coordinate_s:
        double y
        double z

    ctypedef Coordinate_s* Coordinate

    void coord_free(Coordinate c)

    # coordinate array

    ctypedef struct CoArray:
        pass

    CoArray coarray_new(int n, double *y, double *z)

    void coarray_free(CoArray a)

    Coordinate coarray_get(CoArray a, int i)

    int coarray_length(CoArray a)

    double coarray_max_y(CoArray a)

    double coarray_min_y(CoArray a)

    CoArray coarray_subarray_y(CoArray a, double y)

    # cross section properties

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

    # crosssection

    ctypedef struct CrossSection:
        pass

    CrossSection xs_new(CoArray ca,
                        int n_roughness,
                        double *roughness,
                        double *z_roughness)

    void xs_free(CrossSection xs)

    CoArray xs_coarray(CrossSection xs)

    double xs_critical_depth(CrossSection xs, double qc, double y0)

    CrossSectionProps xs_hydraulic_properties(CrossSection xs, double h)

    double xs_normal_depth(CrossSection xs, double qn, double s, double y0)
