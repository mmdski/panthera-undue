cdef extern from "panthera/coordinate.h":

    cdef struct Coordinate_s:
        double y
        double z

    ctypedef Coordinate_s* Coordinate

    void coord_free(Coordinate c)
