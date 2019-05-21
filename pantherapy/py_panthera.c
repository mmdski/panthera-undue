#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#include <numpy/arrayobject.h>

#include <panthera/cii/mem.h>
#include <panthera/crosssection.h>
#include <panthera/exceptions.h>
#include <panthera/reach.h>
#include <panthera/xstable.h>

/*
 * PyXS implementation
 */

typedef struct {
    PyObject_HEAD /* */
        CrossSection xs;
} PyXSObject;

static void
PyXS_dealloc (PyXSObject *self)
{
    if (self->xs)
        xs_free (self->xs);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyXS_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyXSObject *self;
    self     = (PyXSObject *) type->tp_alloc (type, 0);
    self->xs = NULL;
    return (PyObject *) self;
}

static int
PyXS_init (PyXSObject *self, PyObject *args, PyObject *kwds)
{
    int     y_size;
    int     z_size;
    int     n_roughness;
    double *y_data;
    double *z_data;
    double *roughness_data;
    double *z_roughness_data;

    PyObject *y_array;
    PyObject *z_array;
    PyObject *roughness_array;
    PyObject *z_roughness_array = NULL;

    PyObject *   y = NULL, *z = NULL, *roughness = NULL, *z_roughness = NULL;
    static char *kwlist[] = { "y", "z", "roughness", "z_roughness", NULL };

    CoArray ca = NULL;

    if (!PyArg_ParseTupleAndKeywords (
            args, kwds, "OOO|O", kwlist, &y, &z, &roughness, &z_roughness))
        return -1;

    y_array = PyArray_FROM_OTF (y, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (y_array == NULL)
        goto fail;

    z_array = PyArray_FROM_OTF (z, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (z_array == NULL)
        goto fail;

    roughness_array =
        PyArray_FROM_OTF (roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (roughness_array == NULL)
        goto fail;

    /* if z_roughness is specified, get an array */
    if (z_roughness != NULL) {
        z_roughness_array =
            PyArray_FROM_OTF (z_roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
        if (z_roughness_array == NULL)
            goto fail;
    } else {
        z_roughness_array = NULL;
    }

    /* y and z must be 1D */
    if (PyArray_NDIM ((PyArrayObject *) y_array) != 1 ||
        PyArray_NDIM ((PyArrayObject *) z_array) != 1) {
        PyErr_SetString (PyExc_ValueError, "y and z must be 1D arrays");
        goto fail;
    }

    /* y and z must be the same size */
    y_size = PyArray_SIZE ((PyArrayObject *) y_array);
    z_size = PyArray_SIZE ((PyArrayObject *) z_array);
    if (y_size != z_size) {
        PyErr_SetString (PyExc_ValueError,
                         "the size of y and z must be equal");
        goto fail;
    }

    /* size must be greater than two */
    if (y_size < 2) {
        PyErr_SetString (PyExc_ValueError,
                         "the length of y and z must be greater than 2");
        goto fail;
    }

    /* roughness must be greater than equal to zero */
    n_roughness = PyArray_SIZE ((PyArrayObject *) roughness_array);
    if (n_roughness < 1) {
        PyErr_SetString (PyExc_ValueError,
                         "there must be at least one roughness value");
        goto fail;
    }

    y_data = (double *) PyArray_DATA ((PyArrayObject *) y_array);
    z_data = (double *) PyArray_DATA ((PyArrayObject *) z_array);
    roughness_data =
        (double *) PyArray_DATA ((PyArrayObject *) roughness_array);

    /* get a pointer to the z_roughness data if z_roughness was specified */
    if (z_roughness_array != NULL) {
        z_roughness_data =
            (double *) PyArray_DATA ((PyArrayObject *) z_roughness_array);
    } else {
        z_roughness_data = NULL;
    }

    TRY { ca = coarray_new (y_size, y_data, z_data); }
    EXCEPT (coarray_z_order_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "z must be in ascending or equal order");
        goto fail;
    }
    END_TRY;

    TRY
    {
        self->xs = xs_new (ca, n_roughness, roughness_data, z_roughness_data);
    }
    EXCEPT (value_arg_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "Roughness values must be greater than zero");
        goto fail;
    }
    EXCEPT (null_ptr_arg_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "z_roughess must be specified if len(roughness) > 1");
        goto fail;
    }
    END_TRY;

    Py_DECREF (y_array);
    Py_DECREF (z_array);
    Py_DECREF (roughness_array);
    Py_XDECREF (z_roughness_array);
    coarray_free (ca);

    return 0;

fail:
    Py_XDECREF (y_array);
    Py_XDECREF (z_array);
    Py_XDECREF (roughness_array);
    Py_XDECREF (z_roughness_array);

    if (ca)
        coarray_free (ca);

    return -1;
}

static PyObject *
PyXS_property (PyXSObject *self, PyObject *args, xs_prop xs_property)
{
    PyObject *depth_arg   = NULL;
    PyObject *depth_array = NULL;

    PyObject *property          = NULL;
    double *  property_data_ptr = NULL;

    PyArrayIterObject *iter;

    CrossSectionProps xs_props = NULL;

    if (!PyArg_ParseTuple (args, "O", &depth_arg))
        return NULL;

    depth_array =
        PyArray_FROM_OTF (depth_arg, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (depth_array == NULL)
        goto fail;

    property = PyArray_NewLikeArray (
        (PyArrayObject *) depth_array, NPY_CORDER, NULL, 1);
    property_data_ptr = (double *) PyArray_DATA ((PyArrayObject *) property);

    iter = (PyArrayIterObject *) PyArray_IterNew (depth_array);
    if (iter == NULL)
        goto fail;

    while (iter->index < iter->size) {
        TRY
        {
            xs_props =
                xs_hydraulic_properties (self->xs, *(double *) iter->dataptr);
            *(property_data_ptr + (int) iter->index) =
                xsp_get (xs_props, xs_property);
        }
        EXCEPT (xsp_depth_error);
        {
            PyErr_SetString (PyExc_ValueError,
                             "Depth values must be greater than or equal to "
                             "lowest z value in cross section");
            goto fail;
        }
        END_TRY;

        xsp_free (xs_props);
        PyArray_ITER_NEXT (iter);
    }
    Py_DECREF (depth_array);
    Py_DECREF (iter);

    return PyArray_Return ((PyArrayObject *) property);

fail:

    Py_XDECREF (depth_array);
    Py_XDECREF (property);

    return NULL;
}

PyDoc_STRVAR (xs_area__doc__,
              "area($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes area\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of computed area\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed area\n");

static PyObject *
PyXS_area (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_AREA);
}

PyDoc_STRVAR (xs_conveyance__doc__,
              "conveyance($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes conveyance\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of computed conveyance\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed conveyance\n\n"
              "Notes\n"
              "-----\n"
              "Cross section conveyance is computed as "
              "the sum of the conveyance of\n"
              "the subsections within the cross section\n");

static PyObject *
PyXS_conveyance (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CONVEYANCE);
}

PyDoc_STRVAR (xs_coordinates__doc__,
              "coordinates($self, /)\n"
              "--\n"
              "\n"
              "Returns cross section coordinates\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray, numpy.ndarray\n"
              "    Tuple of arrays containing y, z values of cross section "
              "coordinates\n");

static PyObject *
PyXS_coordinates (PyXSObject *self, PyObject *Py_UNUSED (ignored))
{
    CoArray ca;

    PyObject *z;
    PyObject *y;

    int      i;
    int      nd = 1;
    int      size;
    npy_intp ndims;
    double   y_coordinate;
    double   z_coordinate;
    double * y_data_ptr;
    double * z_data_ptr;

    PyObject *rslt;

    ca    = xs_coarray (self->xs);
    size  = coarray_length (ca);
    ndims = size;

    y          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    y_data_ptr = PyArray_DATA ((PyArrayObject *) y);

    z          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    z_data_ptr = PyArray_DATA ((PyArrayObject *) z);

    for (i = 0; i < size; i++) {
        y_coordinate      = coarray_get_y (ca, i);
        *(y_data_ptr + i) = y_coordinate;

        z_coordinate      = coarray_get_z (ca, i);
        *(z_data_ptr + i) = z_coordinate;
    }

    coarray_free (ca);

    if (!(rslt = Py_BuildValue ("(OO)", y, z))) {
        Py_DECREF (y);
        Py_DECREF (z);
        return NULL;
    }

    return rslt;
}

PyDoc_STRVAR (xs_critical_flow__doc__,
              "critical_flow($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes critical flow\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of computed critical flow\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed critical flow\n");

static PyObject *
PyXS_critical_flow (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CRITICAL_FLOW);
}

PyDoc_STRVAR (xs_hydraulic_depth__doc__,
              "hydraulic_depth($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes hydraulic depth\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of hydraulic depth\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed hydraulic depth\n");

static PyObject *
PyXS_hydraulic_depth (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_HYDRAULIC_DEPTH);
}

PyDoc_STRVAR (xs_hydraulic_radius__doc__,
              "hydraulic_radius($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes hydraulic radius\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of hydraulic radius\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed hydraulic radius\n");

static PyObject *
PyXS_hydraulic_radius (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_HYDRAULIC_RADIUS);
}

PyDoc_STRVAR (xs_top_width__doc__,
              "top_width($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes top width\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of computed top width\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed top width\n");

static PyObject *
PyXS_top_width (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_TOP_WIDTH);
}

PyDoc_STRVAR (xs_tw_array__doc__,
              "tw_array($self, elevation, /)\n"
              "--\n"
              "\n"
              "Returns coordinates of top width\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of top width\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray, numpy.ndarray\n"
              "    Tuple of arrays containing y, z values of top width"
              "coordinates\n");

static PyObject *
PyXS_tw_array (PyXSObject *self, PyObject *args)
{
    double depth;

    CoArray ca;
    CoArray wp;

    PyObject *z;
    PyObject *y;

    int      i;
    int      nd = 1;
    int      size;
    npy_intp ndims;
    double   z_wp;
    double * y_data_ptr;
    double * z_data_ptr;

    PyObject *rslt;

    if (!PyArg_ParseTuple (args, "d", &depth))
        return NULL;

    ca    = xs_coarray (self->xs);
    wp    = coarray_subarray_y (ca, depth);
    size  = coarray_length (wp);
    ndims = size;

    y          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    y_data_ptr = PyArray_DATA ((PyArrayObject *) y);

    z          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    z_data_ptr = PyArray_DATA ((PyArrayObject *) z);

    for (i = 0; i < size; i++) {
        z_wp              = coarray_get_z (wp, i);
        *(z_data_ptr + i) = z_wp;
        if (isnan (z_wp))
            *(y_data_ptr + i) = NAN;
        else
            *(y_data_ptr + i) = depth;
    }

    coarray_free (ca);
    coarray_free (wp);

    if (!(rslt = Py_BuildValue ("(OO)", y, z))) {
        Py_DECREF (y);
        Py_DECREF (z);
        return NULL;
    }

    return rslt;
}

PyDoc_STRVAR (xs_velocity_coeff__doc__,
              "velocity_coeff($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes velocity coefficient\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of velocity coefficient\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed velocity coefficient\n");

static PyObject *
PyXS_velocity_coeff (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_VELOCITY_COEFF);
}

PyDoc_STRVAR (xs_wetted_perimeter__doc__,
              "wetted_perimeter($self, elevation, /)\n"
              "--\n"
              "\n"
              "Computes wetted perimeter\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of wetted perimeter\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray\n"
              "    Computed wetted perimeter\n");

static PyObject *
PyXS_wetted_perimeter (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_WETTED_PERIMETER);
}

PyDoc_STRVAR (xs_wp_array__doc__,
              "wp_array($self, elevation, /)\n"
              "--\n"
              "\n"
              "Returns coordinates of wetted perimeter\n\n"
              "Parameters\n"
              "----------\n"
              "elevation : array_like\n"
              "    Elevation of wetted perimeter\n\n"
              "Returns\n"
              "-------\n"
              "numpy.ndarray, numpy.ndarray\n"
              "    Tuple of arrays containing y, z values of wetted perimeter "
              "coordinates\n");

static PyObject *
PyXS_wp_array (PyXSObject *self, PyObject *args)
{
    double depth;

    CoArray ca;
    CoArray wp;

    PyObject *z;
    PyObject *y;

    int      i;
    int      nd = 1;
    int      size;
    npy_intp ndims;
    double * y_data_ptr;
    double * z_data_ptr;

    PyObject *rslt;

    if (!PyArg_ParseTuple (args, "d", &depth))
        return NULL;

    ca    = xs_coarray (self->xs);
    wp    = coarray_subarray_y (ca, depth);
    size  = coarray_length (wp);
    ndims = size;

    y          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    y_data_ptr = PyArray_DATA ((PyArrayObject *) y);

    z          = PyArray_SimpleNew (nd, &ndims, NPY_DOUBLE);
    z_data_ptr = PyArray_DATA ((PyArrayObject *) z);

    for (i = 0; i < size; i++) {
        *(y_data_ptr + i) = coarray_get_y (wp, i);
        *(z_data_ptr + i) = coarray_get_z (wp, i);
    }
    coarray_free (ca);
    coarray_free (wp);

    if (!(rslt = Py_BuildValue ("(OO)", y, z))) {
        Py_DECREF (y);
        Py_DECREF (z);
        return NULL;
    }

    return rslt;
}

static PyMethodDef PyXS_methods[] = {
    { "area", (PyCFunction) PyXS_area, METH_VARARGS, xs_area__doc__ },

    { "conveyance",
      (PyCFunction) PyXS_conveyance,
      METH_VARARGS,
      xs_conveyance__doc__ },

    { "coordinates",
      (PyCFunction) PyXS_coordinates,
      METH_VARARGS,
      xs_coordinates__doc__ },

    { "critical_flow",
      (PyCFunction) PyXS_critical_flow,
      METH_VARARGS,
      xs_critical_flow__doc__ },

    { "hydraulic_depth",
      (PyCFunction) PyXS_hydraulic_depth,
      METH_VARARGS,
      xs_hydraulic_depth__doc__ },

    { "hydraulic_radius",
      (PyCFunction) PyXS_hydraulic_radius,
      METH_VARARGS,
      xs_hydraulic_radius__doc__ },

    { "top_width",
      (PyCFunction) PyXS_top_width,
      METH_VARARGS,
      xs_top_width__doc__ },

    { "tw_array",
      (PyCFunction) PyXS_tw_array,
      METH_VARARGS,
      xs_tw_array__doc__ },

    { "velocity_coeff",
      (PyCFunction) PyXS_velocity_coeff,
      METH_VARARGS,
      xs_velocity_coeff__doc__ },

    { "wetted_perimeter",
      (PyCFunction) PyXS_wetted_perimeter,
      METH_VARARGS,
      xs_wetted_perimeter__doc__ },

    { "wp_array",
      (PyCFunction) PyXS_wp_array,
      METH_VARARGS,
      xs_wp_array__doc__ },

    { NULL }
};

PyDoc_STRVAR (
    xs_doc,
    "CrossSection(y, x, roughness) -> new CrossSection with one subsection\n"
    "CrossSection(y, x, roughness, z_roughness) -> new CrossSection with\n"
    "    len(roughness) subsections\n\n"
    "Hydraulic cross section\n\n"
    "Parameters\n"
    "----------\n"
    "y : array_like\n"
    "    Vertical values of cross section coordinates\n"
    "z : array_like\n"
    "    Lateral values of cross section coordinates\n"
    "roughness : array_like\n"
    "    Manning coefficient for cross section subsections\n"
    "z_roughness : array_like, optional\n"
    "    z values of cross section subsections defined by\n"
    "    roughness values (optional). If `roughness` contains\n"
    "    more than one element, `z_roughness` must be passed and\n"
    "    contain one less element than `roughness`.\n");

static PyTypeObject PyXSType = {
    PyVarObject_HEAD_INIT (NULL, 0).tp_name =
        "pantherapy.panthera.CrossSection",
    .tp_doc       = xs_doc,
    .tp_basicsize = sizeof (PyXSObject),
    .tp_itemsize  = 0,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_new       = PyXS_new,
    .tp_init      = (initproc) PyXS_init,
    .tp_dealloc   = (destructor) PyXS_dealloc,
    .tp_methods   = PyXS_methods,
};

/*
 * PyReach implementation
 */

typedef struct {
    PyObject_HEAD /* */
        Reach reach;
} PyReachObject;

static void
PyReach_dealloc (PyReachObject *self)
{
    if (self->reach)
        reach_free (self->reach);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyReach_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyReachObject *self;
    self        = (PyReachObject *) type->tp_alloc (type, 0);
    self->reach = NULL;
    return (PyObject *) self;
}

static int
PyReach_init (PyReachObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *x_array = NULL, *y_array = NULL, *xs_number_array = NULL,
             *xs_table_items = NULL, *iterator = NULL, *item = NULL,
             *key = NULL, *value = NULL;

    int         x_size;
    int         y_size;
    int         xs_num_size;
    PyXSObject *py_xs    = NULL;
    XSTable     xs_table = NULL;
    int         xs_key;
    double *    x_array_data;
    double *    y_array_data;
    int *       xs_number_data;

    /* create array objects from arguments */
    PyObject *x, *y, *xs_number, *xs_table_ob;
    if (!PyArg_ParseTuple (args, "OOOO", &x, &y, &xs_number, &xs_table_ob))
        return -1;

    x_array = PyArray_FROM_OTF (x, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (x_array == NULL)
        goto fail;

    y_array = PyArray_FROM_OTF (y, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (y_array == NULL)
        goto fail;

    xs_number_array =
        PyArray_FROM_OTF (xs_number, NPY_INT, NPY_ARRAY_C_CONTIGUOUS);
    if (xs_number_array == NULL)
        goto fail;

    /* x, y, and xs numbers must be 1D */
    if (PyArray_NDIM ((PyArrayObject *) x_array) != 1) {
        PyErr_SetString (PyExc_ValueError, "'x' must be 1D");
        goto fail;
    }
    if (PyArray_NDIM ((PyArrayObject *) y_array) != 1) {
        PyErr_SetString (PyExc_ValueError, "'y' must be 1D");
        goto fail;
    }
    if (PyArray_NDIM ((PyArrayObject *) xs_number_array) != 1) {
        PyErr_SetString (PyExc_ValueError, "'xs_number' must be 1D");
        goto fail;
    }

    /* x, y, and xs numbers must be the same size */
    x_size      = PyArray_SIZE ((PyArrayObject *) x_array);
    y_size      = PyArray_SIZE ((PyArrayObject *) y_array);
    xs_num_size = PyArray_SIZE ((PyArrayObject *) xs_number_array);
    if (x_size != y_size || x_size != xs_num_size) {
        PyErr_SetString (PyExc_ValueError,
                         "The size of x, y, and xs_number must be equal");
        goto fail;
    }

    /* create a cross section table for the call to reach_new */
    xs_table       = xstable_new ();
    xs_table_items = PyObject_CallMethod (xs_table_ob, "items", NULL);
    if (!xs_table_items)
        goto fail;
    iterator = PyObject_GetIter (xs_table_items);
    if (!xs_table_items)
        goto fail;
    while ((item = PyIter_Next (iterator))) {
        /* get the key, value pair from the item tuple */
        if (!(key = PySequence_GetItem (item, 0)))
            goto fail;
        if (!(value = PySequence_GetItem (item, 1)))
            goto fail;

        /* get the key as an integer */
        xs_key = (int) PyLong_AsLong (key);
        if (PyErr_Occurred ())
            goto fail;

        /* get the value as a cross section */
        if (!PyObject_TypeCheck (value, &PyXSType)) {
            PyErr_SetString (PyExc_TypeError,
                             "xs_table values must be cross section type");
            goto fail;
        }
        py_xs = (PyXSObject *) value;

        xstable_put (xs_table, xs_key, py_xs->xs);

        Py_DECREF (key);
        Py_DECREF (value);
        Py_DECREF (item);
    }

    Py_DECREF (iterator);

    if (PyErr_Occurred ())
        goto fail;

    x_array_data   = (double *) PyArray_DATA ((PyArrayObject *) x_array);
    y_array_data   = (double *) PyArray_DATA ((PyArrayObject *) y_array);
    xs_number_data = (int *) PyArray_DATA ((PyArrayObject *) xs_number_array);

    TRY
    {
        self->reach = reach_new (
            x_size, x_array_data, y_array_data, xs_number_data, xs_table);
    }
    EXCEPT (reach_xs_num_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "Values in 'xs_number' must be keys in 'xs_table'");
        goto fail;
    }
    EXCEPT (reach_x_order_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "Values in 'x' must be in ascending or equal order");
        goto fail;
    }
    END_TRY;

    Py_DECREF (x_array);
    Py_DECREF (y_array);
    Py_DECREF (xs_number_array);

    return 0;

fail:
    Py_XDECREF (x_array);
    Py_XDECREF (y_array);
    Py_XDECREF (xs_number_array);
    Py_XDECREF (key);
    Py_XDECREF (value);
    Py_XDECREF (item);
    Py_XDECREF (iterator);
    if (xs_table)
        xstable_free (xs_table);
    return -1;
}

PyDoc_STRVAR (
    reach_doc,
    "reach(x, y, xs_number, xs_table) -> new reach\n\n"
    "River reach\n\n"
    "Parameters\n"
    "----------\n"
    "x : array_like\n"
    "    Longitudinal distances of each node\n"
    "y : array_like\n"
    "    Vertical location of the thalweg of each node\n"
    "xs_number : array_like\n"
    "    Number of the cross section in each node. The number must\n"
    "    correspond to a key in `xs_table`\n"
    "xs_table : dict_like\n"
    "    `dict` containing key, value pairs of cross section key (`int`),\n"
    "    :class:`pantherapy.panthera.CrossSection`\n\n"
    "Notes\n"
    "-----\n"
    "`x`, `y`, and `xs_number` must have the same length.\n\n"
    "`xs_table` must have an `items` method that returns an iterator with\n"
    "key, value pairs as elements (like a `dict`)\n");

static PyTypeObject PyReachType = {
    PyVarObject_HEAD_INIT (NULL, 0).tp_name = "pantherapy.panthera.Reach",
    .tp_doc                                 = reach_doc,
    .tp_basicsize                           = sizeof (PyReachObject),
    .tp_itemsize                            = 0,
    .tp_flags                               = Py_TPFLAGS_DEFAULT,
    .tp_new                                 = PyReach_new,
    .tp_init                                = (initproc) PyReach_init,
    .tp_dealloc                             = (destructor) PyReach_dealloc,
};

static PyModuleDef pantheramodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "panthera",
    .m_doc  = "Panthera hydraulics module",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_panthera (void)
{
    PyObject *m;
    if (PyType_Ready (&PyXSType) < 0)
        return NULL;
    if (PyType_Ready (&PyReachType) < 0)
        return NULL;

    m = PyModule_Create (&pantheramodule);
    if (m == NULL)
        return NULL;
    import_array ();

    Py_INCREF (&PyXSType);
    PyModule_AddObject (m, "CrossSection", (PyObject *) &PyXSType);
    PyModule_AddObject (m, "Reach", (PyObject *) &PyReachType);
    return m;
}
