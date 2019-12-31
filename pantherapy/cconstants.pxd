cdef extern from "panthera/constants.h":
    double const_gravity()
    double const_manning()
    void const_set_gravity(double gravity)
    void const_set_manning(double k)
