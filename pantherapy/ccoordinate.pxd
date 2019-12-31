cdef extern from "panthera/coordinate.h":

    cdef struct Coordinate:
        double y
        double z

    ctypedef Coordinate* Coordinate

    void coord_free(Coordinate c)
