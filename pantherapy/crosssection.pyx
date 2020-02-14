#  cython : language_level=3

from cpython.ref cimport PyObject
cimport cpython.float as pyfloat
from libc.math cimport isfinite, sqrt, NAN

cimport numpy as cnp
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

cimport pantherapy.cconstants as constants
cimport pantherapy.ccrosssection as cxs

cdef class CrossSection:
    """CrossSection(y, z, roughness) -> new CrossSection with one subsection

    Hydraulic cross section

    Parameters
    ----------
    y : array_like
        Vertical values of cross section coordinates
    z : array_like
        Horizontal values of cross section coordinates
    roughness : float
        Manning coefficient for cross section

    """

    cdef cxs.CrossSection xs

    def __init__(self, y, z, roughness):

        y = np.array(y, dtype=np.float64, order='C')
        z = np.array(z, dtype=np.float64, order='C')

        roughness = float(roughness)

        if not np.all(z[:-1] <= z[1:]):
            raise ValueError("z must be in ascending order")
        if np.ndim(y) != 1 or np.ndim(z) != 1:
            raise ValueError("y and z must be one-dimensional")
        if not y.size == z.size:
            raise ValueError("y and z must be the same size")
        if not y.size > 2 or not z.size > 2:
            raise ValueError("the length of y and z must be greater than 2")

        cdef double n = pyfloat.PyFloat_AsDouble(roughness)

        cdef int n_coordinates = y.size

        cdef double[:] y_view = y
        cdef double[:] z_view = z

        cdef cxs.CoArray ca = \
            cxs.coarray_new(n_coordinates, &y_view[0], &z_view[0])

        self.xs = cxs.xs_new(ca, 1, &n, NULL)

        cxs.coarray_free(ca)

    def __dealloc__(self):
        cxs.xs_free(self.xs)

    def _plot_tw_wp(self, cy, ax):

        cdef cxs.CoArray ca = cxs.xs_coarray(self.xs)
        cdef cxs.CoArray wp_array
        cdef cxs.Coordinate c

        wp_array = cxs.coarray_subarray_y(ca, cy)

        cdef Py_ssize_t i
        cdef Py_ssize_t length = cxs.coarray_length(wp_array)

        tw_y = np.zeros(length)
        wp_y = np.zeros(length)
        z = np.zeros(length)

        cdef double[:] tw_y_view = tw_y
        cdef double[:] wp_y_view = wp_y
        cdef double[:] z_view = z

        for i in range(length):
            c = cxs.coarray_get(wp_array, <int> i)
            tw_y_view[i] = cy
            wp_y_view[i] = c.y
            z_view[i] = c.z
            cxs.coord_free(c)

        xs_area_zy = [*zip(z, wp_y)]

        if cy > wp_y_view[0]:
            xs_area_zy.insert(0, (z[0], cy))
        if cy > wp_y_view[-1]:
            xs_area_zy.append((z[-1], cy))

        if len(xs_area_zy) > 2:
            poly = Polygon(xs_area_zy, facecolor='b',
                            alpha=0.25, label='Wetted area')
            ax.add_patch(poly)

        ax.plot(z, tw_y, 'b', linewidth=2.5, label='Top width')
        ax.plot(z, wp_y, 'g', linewidth=5, label='Wetted perimeter')

        cxs.coarray_free(wp_array)

    cdef _property(self, y, cxs.xs_prop prop):

        y = np.array(y, dtype=np.float64, order='C')
        p = np.zeros_like(y)

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = y.size

        cdef double *y_data = <double *> cnp.PyArray_DATA(y)
        cdef double *p_data = <double *> cnp.PyArray_DATA(p)

        cdef cxs.CrossSectionProps xsp
        cdef double result

        for i in range(i_max):
            if not isfinite(y_data[i]):
                p_data[i] = NAN
            else:
                xsp = cxs.xs_hydraulic_properties(self.xs, y_data[i])
                result = cxs.xsp_get(xsp, prop)
                p_data[i] = result
                cxs.xsp_free(xsp)

        if np.ndim(p) > 0:
            return p
        else:
            return p_data[0]

    def area(self, y):
        """area(y)

        Computes area

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed area

        """

        return self._property(y, cxs.XS_AREA)

    def conveyance(self, y):
        """conveyance(y)

        Computes conveyance

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed conveyance

        Notes
        -----
        Cross section conveyance is computed as the sum of the
        conveyance of the subsections within the cross section

        """

        return self._property(y, cxs.XS_CONVEYANCE)

    def coordinates(self):
        """Returns cross section coordinates

        Returns
        -------
        numpy.ndarray, numpy.ndarray
            Tuple of arrays containing y, z values of cross section
            coordinates

        """

        cdef cxs.CoArray ca = cxs.xs_coarray(self.xs)
        cdef Py_ssize_t length = cxs.coarray_length(ca)

        y = np.zeros(length)
        z = np.zeros(length)

        cdef double[:] y_view = y
        cdef double[:] z_view = z

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = length

        cdef cxs.Coordinate c

        for i in range(i_max):
            c = cxs.coarray_get(ca, <int> i)
            y_view[i] = c.y
            z_view[i] = c.z
            cxs.coord_free(c)

        cxs.coarray_free(ca)

        return y, z

    def critical_depth(self, critical_flow, y0=None):
        """critical_depth(critical_flow, y0=None)

        Computes critical depth

        Parameters
        ----------
        critical_flow : array_like
            Critical flow for computing critical depth
        y0 : float, optional
            Initial estimate of critical depth

        Returns
        -------
        numpy.ndarray
            Computed critical depth

        """

        critical_flow = np.array(critical_flow, dtype=np.float64, order='C')

        cdef double cy0
        cdef cxs.CoArray ca
        cdef double y_min
        cdef double y_max

        if y0 is None:
            ca = cxs.xs_coarray(self.xs)
            y_min = cxs.coarray_min_y(ca)
            y_max = cxs.coarray_max_y(ca)
            cy0 = 0.75 * (y_max - y_min) + y_min
            cxs.coarray_free(ca)
        else:
            if not pyfloat.PyFloat_Check(y0):
                raise ValueError("y0 must be a float")
            cy0 = pyfloat.PyFloat_AsDouble(y0)

        critical_depth = np.zeros_like(critical_flow)

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = critical_flow.size

        cdef double *qc_data = <double *> cnp.PyArray_DATA(critical_flow)
        cdef double *yc_data = <double *> cnp.PyArray_DATA(critical_depth)

        for i in range(i_max):
            yc_data[i] = cxs.xs_critical_depth(self.xs, qc_data[i], cy0)

        if np.ndim(critical_depth) > 0:
            return critical_depth
        else:
            return yc_data[0]

    def critical_flow(self, y):
        """critical_flow(y)

        Computes critical flow

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed critical flow

        """

        return self._property(y, cxs.XS_CRITICAL_FLOW)

    def hydraulic_depth(self, y):
        """hydrauilc_depth(y)

        Computes hydraulic depth

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed hydraulic depth

        """

        return self._property(y, cxs.XS_HYDRAULIC_DEPTH)

    def hydraulic_radius(self, y):
        """hydraulic_radius(y)

        Computes hydraulic radius

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed hydraulic radius

        """

        return self._property(y, cxs.XS_HYDRAULIC_RADIUS)

    def normal_depth(self, normal_flow, slope, y0=None):
        """normal_depth(normal_flow, slope, y0=None)

        Computes normal depth

        Parameters
        ----------
        normal_flow : array_like
            Normal flow for computing normal depth
        slope : float
            Bed slope
        y0 : float, optional
            Initial estimate of normal depth

        Returns
        -------
        numpy.ndarray
            Computed normal depth

        """

        normal_flow = np.array(normal_flow, dtype=np.float64, order='C')

        if not pyfloat.PyFloat_Check(slope):
            raise ValueError("slope must be a float")

        cdef double cy0
        cdef cxs.CoArray ca
        cdef double y_min
        cdef double y_max

        if y0 is None:
            ca = cxs.xs_coarray(self.xs)
            y_min = cxs.coarray_min_y(ca)
            y_max = cxs.coarray_max_y(ca)
            cy0 = 0.75 * (y_max - y_min) + y_min
            cxs.coarray_free(ca)
        else:
            if not pyfloat.PyFloat_Check(y0):
                raise ValueError("y0 must be a float")
            cy0 = pyfloat.PyFloat_AsDouble(y0)

        normal_depth = np.zeros_like(normal_flow)

        cdef double s = pyfloat.PyFloat_AsDouble(slope)

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = normal_flow.size

        cdef double *qn_data = <double *> cnp.PyArray_DATA(normal_flow)
        cdef double *yn_data = <double *> cnp.PyArray_DATA(normal_depth)

        for i in range(i_max):
            yn_data[i] = cxs.xs_normal_depth(self.xs, qn_data[i], s, cy0)

        if np.ndim(normal_depth) > 0:
            return normal_depth
        else:
            return yn_data[0]

    def normal_flow(self, y, slope):
        """normal_flow(y, slope)

        Computes normal flow

        Parameters
        ----------
        y : array_like
            Water surface elevation
        slope : float
            Bed slope

        Returns
        -------
        numpy.ndarray
            Computed normal flow

        """

        y = np.array(y, dtype=np.float64, order='C')

        if not pyfloat.PyFloat_Check(slope):
            raise ValueError("slope must be a float")

        normal_flow = np.zeros_like(y)

        cdef double sqrt_s = sqrt(pyfloat.PyFloat_AsDouble(slope))

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = y.size

        cdef double *y_data = <double *> cnp.PyArray_DATA(y)
        cdef double *qn_data = <double *> cnp.PyArray_DATA(normal_flow)

        cdef cxs.CrossSectionProps xsp
        cdef double conveyance

        for i in range(i_max):
            if not isfinite(y_data[i]):
                qn_data[i] = NAN
            else:
                xsp = cxs.xs_hydraulic_properties(self.xs, y_data[i])
                conveyance = cxs.xsp_get(xsp, cxs.XS_CONVEYANCE)
                cxs.xsp_free(xsp)
                qn_data[i] = conveyance * sqrt_s

        if np.ndim(normal_flow) > 0:
            return normal_flow
        else:
            return qn_data[0]

    def plot(self, y=None, ax=None):
        """plot(y=None, ax=None)

        Plot cross section geometry

        Parameters
        ----------
        y : float, optional
            Water surface elevation relative to cross section coordinate
            elevations, optional (default is None). If not None, wetted
            properties will be plotted if y is greater than the lowest
            y-coordinate in the cross section.
        ax : matplotlib.axes.Axes, optional
            Plot axes, optional (the default is None, which creates a
            new Axes instance).

        Returns
        -------
        matplotlib.axes.Axes
            Plot axes

        """

        if ax is None:
            ax = plt.axes()

        y_coord, z_coord = self.coordinates()
        ax.plot(z_coord, y_coord, 'k', marker='.', label='Coordinates')

        cdef double cy
        cdef double min_y
        cdef cxs.CoArray ca

        if y is not None:
            y = float(y)

            ca = cxs.xs_coarray(self.xs)
            min_y = cxs.coarray_min_y(ca)

            cy = pyfloat.PyFloat_AsDouble(y)

            if cy > min_y:
                self._plot_tw_wp(cy, ax)

            cxs.coarray_free(ca)

        ax.set_xlabel('z')
        ax.set_ylabel('y')

        ax.legend()

        return ax

    def specific_energy(self, y, q):
        """specific_energy(y, q)

        Computes specific energy

        Parameters
        ----------
        y : array_like
            Water surface elevation
        q : float
            Discharge

        Returns
        -------
        numpy.ndarray
            Computed specific energy

        """

        y = np.array(y, dtype=np.float64, order='C')

        q = float(q)

        specific_energy = np.zeros_like(y)

        cdef cq = pyfloat.PyFloat_AsDouble(q)

        cdef Py_ssize_t i
        cdef Py_ssize_t i_max = y.size

        cdef double alpha
        cdef double area
        cdef double gravity = constants.const_gravity()
        cdef cxs.CrossSectionProps xsp

        cdef double *y_data = <double *> cnp.PyArray_DATA(y)
        cdef double *e_data = <double *> cnp.PyArray_DATA(specific_energy)

        for i in range(i_max):
            if not isfinite(y_data[i]):
                e_data[i] = NAN
            else:
                xsp = cxs.xs_hydraulic_properties(self.xs, y_data[i])
                alpha = cxs.xsp_get(xsp, cxs.XS_VELOCITY_COEFF)
                area = cxs.xsp_get(xsp, cxs.XS_AREA)
                cxs.xsp_free(xsp)
                e_data[i] = \
                    y_data[i] + alpha * cq * cq / (2 * gravity * area * area)

        if np.ndim(specific_energy) > 0:
            return specific_energy
        else:
            return e_data[0]

    def top_width(self, y):
        """top_width(y)

        Computes top width

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed top width

        """

        return self._property(y, cxs.XS_TOP_WIDTH)

    def velocity_coeff(self, y):
        """velocity_coeff(y)

        Computes velocity coefficient

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed velocity coefficient

        """

        return self._property(y, cxs.XS_VELOCITY_COEFF)

    def wetted_perimeter(self, y):
        """wetted_perimeter(y)

        Computes wetted perimeter

        Parameters
        ----------
        y : array_like
            Water surface elevation

        Returns
        -------
        numpy.ndarray
            Computed wetted perimeter

        """

        return self._property(y, cxs.XS_WETTED_PERIMETER)
