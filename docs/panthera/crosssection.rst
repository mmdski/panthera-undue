=============
Cross section
=============

.. code-block:: c

    pantherapy/crosssection.h

Coordinate
==========

y, z cross section coordinate

.. c:type:: Coordinate

    .. code-block:: c

        struct Coordinate {
            double y;
            double z;
        };

        typedef struct Coordinate *Coordinate;

    .. c:member:: double y

        Vertical location

    .. c:member:: double z

        Lateral location

.. c:function:: Coordinate coord_copy(Coordinate c)

    Creates a new copy of *c*. The returned coordinate is newly created and
    should be freed with :c:func:`coord_free`.

.. c:function:: int coord_eq(Coordinate c1, Coordinate c2)

    Returns 0 if c1 and c2 are equal.

.. c:function:: void coord_free(Coordinate c)

    Frees coordinate *c*.

.. c:function:: Coordinate coord_interp_y(Coordinate c1, Coordinate c2, \
    double z)

    Creates a new coordinate with a y that is linearly interpolated between
    *c1* and *c2* using *z*. The new coordinate will have a vertical value that
    is equal to the interpolated value and a lateral value equal to *z*. The
    returned coordinate is newly created and should be freed with
    :c:func:`coord_free`.

.. c:function:: Coordinate coord_interp_z(Coordinate c1, Coordinate c2, \
    double y)

    Creates a new coordinate with a z that is linearly interpolated between
    *c1* and *c2* using *y*. The new coordinate will have a lateral value that
    is equal to the interpolated value and a vertical value equal to *y*. The
    returned coordinate is newly created and should be freed with
    :c:func:`coord_free`.

.. c:function:: Coordinate coord_new(double y, double z)

    Creates a new coordinate with *y* and *z* as the vertical and lateral
    values. The resulting coordinate is newly created and should be freed with
    :c:func:`coord_free`.


CoArray
=======

Coordinate array

.. c:type:: CoArray

    .. code-block:: c

        typedef struct CoArray *CoArray;

.. c:function:: CoArray coarray_copy(CoArray a)

    Returns a new copy of *a*. The returned coordinate array is newly allocated
    and should be freed with :c:func:`coarray_free` when no longer needed.

.. c:function:: int coarray_eq(CoArray a1, CoArray a2)

    Returns 0 if *a1* and *a2* are equal.

.. c:function:: void coarray_free(CoArray a)

    Frees coordinate array *a*.

.. c:function:: Coordinate coarray_get(CoArray a, int i)

    Returns a copy of the `i`-th coordinate of *a*. The returned coordinate is
    newly created and should be freed with :c:func:`coord_free` when no longer
    needed. :c:func:`coarray_get` may return `NULL` if the `i`-th coordinate
    doesn't exist.

.. c:function:: int coarray_length(CoArray a)

    Returns the number of :c:type:`Coordinate` s in *a*.

.. c:function:: double coarray_max_y(CoArray a)

    Returns the maximum :c:member:`y` value in *a*.

.. c:function:: double coarray_min_y(CoArray a)

    Returns the minimum :c:member:`y` value in *a*.

.. c:function:: CoArray coarray_new(int n, double *y, double *z)

    Creates a new coordinate array of length *n* and y- and z- values of *y*
    and *z*. The resulting coordinate array is newly allocated and should be
    freed with :c:func:`coarray_free`.

.. c:function:: CoArray coarray_subarray(CoArray a, double zlo, double zhi)

    Returns a subset of the coordinates in *a* as a new coordinate array. The
    subset is selected so that the z-values of the coordinates are between
    *zlo* and *zhi*. *zlo* and *zhi* must be within the range of the z values
    of the coordinates contained in *a*, inclusive. The resulting coordinate
    array is newly created and should be freed with :c:func:`coarray_free` when
    no longer needed.

.. c:function:: CoArray coarray_subarray_y(CoArray a, double y)

    Returns a subset of the coordinates in a as a new coordinate array. The
    subset is selected so that the y-values of the coordinates are less than
    *yhi*. Interpolated coordinates are added to the ends of the subarray if
    *yhi* doesn't exactly define the y-values of the first and last coordinates
    in *a*. The resulting coordinate array is newly created and should be freed
    with :c:func:`coarray_free` after use.


CrossSectionProps
=================

Cross section properties

.. c:type:: CrossSectionProps

    .. code-block:: c

        typedef struct CrossSectionProps *CrossSectionProps;

.. c:type:: xs_prop

    .. code-block:: c

        typedef enum {
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
        } xs_prop;

    .. c:member:: XS_DEPTH

        Depth properties are calculated at

    .. c:member:: XS_AREA

    .. c:member:: XS_TOP_WIDTH

    .. c:member:: XS_WETTED_PERIMETER

    .. c:member:: XS_HYDRAULIC_DEPTH

    .. c:member:: XS_HYDRAULIC_RADIUS

    .. c:member:: XS_CONVEYANCE

    .. c:member:: XS_VELOCITY_COEFF

    .. c:member:: XS_CRITICAL_FLOW

    .. c:member:: N_XSP

        Number of cross section properties

.. c:function:: void xsp_free(CrossSectionProps xsp)

    Frees *xsp*.

.. c:function:: double xsp_get(CrossSectionProps xsp, xs_prop prop)

    Returns the value of *prop* stored in *xsp*.


CrossSection
============

.. c:type:: CrossSection

    .. code-block:: c

        typedef struct CrossSection *CrossSection;

.. c:function:: CoArray xs_coarray(CrossSection xs)

    Returns a copy of the coordinate array that defines the coordinates in
    *xs*. The resulting coordinate array is newly created and should be freed
    with :c:func:`coarray_free` after use.

.. c:function:: double xs_critical_depth(CrossSection xs, \
    double critical_flow, double initial_depth)

    Computes critical depth of *xs* at flow *critical_flow* using
    *initial_depth* as an initial depth. Returns `NAN` if no solution is found.

.. c:function:: void xs_free(CrossSection xs)

    Frees *xs*.

.. c:function:: CrossSectionProps xs_hydraulic_properties( \
    CrossSection xs, double y)

    Computes cross section properties at water surface elevation *y*. The
    return cross section properties is newly created and should be freed with
    :c:func:`xsp_free` after use.

.. c:function:: CrossSection xs_new(CoArray ca, int n_roughness, \
    double *roughness, double *z_roughness)

    Creates a new cross section defined by the coordinates in *ca*,
    *n_roughness* subsections, *roughness[i]* for the `i`-th subsection,
    and lateral subsection boundaries defined by *z_roughness*. A new copy of
    *ca* is made. If *n_roughness* is 1, *z_roughness* is ignored and may be
    `NULL`. Otherwise, the length of *z_roughness* must be *n_roughness* - 1.

.. c:function:: double xs_normal_depth(CrossSection xs, double normal_flow, \
    double slope, double initial_depth)

    Computes the normal depth of a cross section at a flow of *normal_flow* and
    bed slope *slope* using an iterative method, with *initial_depth* as an
    initial estimate for elevation. Returns `NAN` if no solution is found.
