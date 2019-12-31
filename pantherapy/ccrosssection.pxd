from pantherapy.ccoarray cimport CoArray
from pantherapy.cxsp cimport CrossSectionProps


cdef extern from "panthera/crosssection.h":

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
