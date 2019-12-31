from pantherapy.ccoordinate cimport Coordinate

cdef extern from "panthera/coarray.h":

    ctypedef struct CoArray:
        pass

    CoArray coarray_new(int n, double *y, double *z)

    void coarray_free(CoArray a)

    Coordinate coarray_get(CoArray a, int i)

    int coarray_length(CoArray a)

    double coarray_max_y(CoArray a)

    double coarray_min_y(CoArray a)

    CoArray coarray_subarray_y(CoArray a, double y)
