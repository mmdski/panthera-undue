#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#include <numpy/arrayobject.h>

#include <panthera/crosssection.h>
#include <panthera/exceptions.h>
#include <panthera/xstable.h>

/*
 * Py_CrossSection type definition
 */

typedef struct {
    PyObject_HEAD /* */
        PyObject *y;
    PyObject *    z;
    PyObject *    roughness;
    PyObject *    z_roughness;
    CrossSection  xs;
} PyXSObject;

static void
PyXS_dealloc (PyXSObject *self)
{
    Py_XDECREF (self->y);
    Py_XDECREF (self->z);
    Py_XDECREF (self->roughness);
    Py_XDECREF (self->z_roughness);
    if (self->xs)
        xs_free (self->xs);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyXS_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyXSObject *self;
    self              = (PyXSObject *) type->tp_alloc (type, 0);
    self->y           = NULL;
    self->z           = NULL;
    self->roughness   = NULL;
    self->z_roughness = NULL;
    self->xs          = NULL;
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

    PyObject *   y = NULL, *z = NULL, *roughness = NULL, *z_roughness = NULL;
    static char *kwlist[] = { "y", "z", "roughness", "z_roughness", NULL };

    CoArray ca;

    if (!PyArg_ParseTupleAndKeywords (
            args, kwds, "OOO|O", kwlist, &y, &z, &roughness, &z_roughness))
        return -1;

    self->y = PyArray_FROM_OTF (y, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (self->y == NULL)
        return -1;

    self->z = PyArray_FROM_OTF (z, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (self->z == NULL)
        return -1;

    self->roughness =
        PyArray_FROM_OTF (roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (self->roughness == NULL)
        return -1;

    /* if z_roughness is specified, get an array */
    if (z_roughness != NULL) {
        self->z_roughness =
            PyArray_FROM_OTF (z_roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
        if (self->z_roughness == NULL)
            return -1;
    } else {
        self->z_roughness = NULL;
    }

    /* y and z must be 1D */
    if (PyArray_NDIM ((PyArrayObject *) self->y) != 1 ||
        PyArray_NDIM ((PyArrayObject *) self->z) != 1) {
        PyErr_SetString (PyExc_ValueError, "y and z must be 1D arrays");
        return -1;
    }

    /* y and z must be the same size */
    y_size = PyArray_SIZE ((PyArrayObject *) self->y);
    z_size = PyArray_SIZE ((PyArrayObject *) self->z);
    if (y_size != z_size) {
        PyErr_SetString (PyExc_ValueError,
                         "the size of y and z must be equal");
        return -1;
    }

    /* size must be greater than two */
    if (y_size < 2) {
        PyErr_SetString (PyExc_ValueError,
                         "the length of y and z must be greater than 2");
        return -1;
    }

    /* roughness must be greater than equal to zero */
    n_roughness = PyArray_SIZE ((PyArrayObject *) self->roughness);
    if (n_roughness < 1) {
        PyErr_SetString (PyExc_ValueError,
                         "there must be at least one roughness value");
    }

    y_data = (double *) PyArray_DATA ((PyArrayObject *) self->y);
    z_data = (double *) PyArray_DATA ((PyArrayObject *) self->z);
    roughness_data =
        (double *) PyArray_DATA ((PyArrayObject *) self->roughness);

    /* get a pointer to the z_roughness data if z_roughness was specified */
    if (z_roughness != NULL) {
        z_roughness_data =
            (double *) PyArray_DATA ((PyArrayObject *) self->z_roughness);
    } else {
        z_roughness_data = NULL;
    }

    TRY { ca = coarray_new (y_size, y_data, z_data); }
    EXCEPT (coarray_z_order_error);
    {
        PyErr_SetString (PyExc_ValueError,
                         "z must be in ascending or equal order");
        return -1;
    }
    END_TRY;

    TRY
    {
        self->xs = xs_new (ca, n_roughness, roughness_data, z_roughness_data);
    }
    EXCEPT (value_arg_error);
    {
        coarray_free (ca);
        PyErr_SetString (PyExc_ValueError,
                         "Roughness values must be greater than zero");
        return -1;
    }
    EXCEPT (null_ptr_arg_error);
    {
        coarray_free (ca);
        PyErr_SetString (PyExc_ValueError,
                         "z_roughess must be specified if len(roughness) > 1");
        return -1;
    }
    END_TRY;

    coarray_free (ca);

    return 0;
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
        return NULL;

    property = PyArray_NewLikeArray (
        (PyArrayObject *) depth_array, NPY_CORDER, NULL, 1);
    property_data_ptr = (double *) PyArray_DATA ((PyArrayObject *) property);

    iter = (PyArrayIterObject *) PyArray_IterNew (depth_array);
    if (iter == NULL) {
        Py_DECREF (depth_array);
        Py_DECREF (property);
        return NULL;
    }

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
            Py_DECREF (depth_array);
            Py_DECREF (property);
            PyErr_SetString (PyExc_ValueError,
                             "Depth values must be greater than or equal to "
                             "lowest z value in cross section");
            return NULL;
        }
        END_TRY;

        xsp_free (xs_props);
        PyArray_ITER_NEXT (iter);
    }
    Py_DECREF (depth_array);

    return PyArray_Return ((PyArrayObject *) property);
}

static PyObject *
PyXS_area (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_AREA);
}

static PyObject *
PyXS_conveyance (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CONVEYANCE);
}

static PyObject *
PyXS_coordinates (PyXSObject *self, PyObject *Py_UNUSED (ignored))
{
    PyObject *y;
    PyObject *z;
    PyObject *rslt;

    y = PyArray_NewCopy ((PyArrayObject *) self->y, NPY_CORDER);
    z = PyArray_NewCopy ((PyArrayObject *) self->z, NPY_CORDER);
    if (!(rslt = Py_BuildValue ("(OO)", y, z)))
        return NULL;

    return rslt;
}

static PyObject *
PyXS_critical_flow (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CRITICAL_FLOW);
}

static PyObject *
PyXS_hydraulic_depth (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_HYDRAULIC_DEPTH);
}

static PyObject *
PyXS_hydraulic_radius (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_HYDRAULIC_RADIUS);
}

static PyObject *
PyXS_top_width (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_TOP_WIDTH);
}

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

    if (!(rslt = Py_BuildValue ("(OO)", y, z)))
        return NULL;

    return rslt;
}

static PyObject *
PyXS_velocity_coeff (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_VELOCITY_COEFF);
}

static PyObject *
PyXS_wetted_perimeter (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_WETTED_PERIMETER);
}

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

    if (!(rslt = Py_BuildValue ("(OO)", y, z)))
        return NULL;

    return rslt;
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

PyDoc_STRVAR (xs_doc,
              "Hydraulic cross section\n\n"
              "Parameters\n"
              "----------\n"
              "y : numpy.ndarray\n"
              "    vertical values of cross section coordinates\n"
              "z : numpy.ndarray\n"
              "    lateral values of cross section coordinates\n"
              "roughness : numpy.ndarray\n"
              "    Manning coefficient for cross section subsections\n"
              "z_roughness : numpy.ndarray, optional\n"
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
 * Py_XSTable type definition
 */

typedef struct {
    PyObject_HEAD /* */
        XSTable xs_table;
} PyXSTableObject;

static void
PyXSTable_dealloc (PyXSTableObject *self)
{
    if (self->xs_table)
        xstable_free (self->xs_table);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyXSTable_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyXSTableObject *self;
    self           = (PyXSTableObject *) type->tp_alloc (type, 0);
    self->xs_table = NULL;
    return (PyObject *) self;
}

static int
PyXSTable_init (PyXSTableObject *self, PyObject *args, PyObject *kwds)
{
    self->xs_table = xstable_new ();

    return 0;
}

static PyObject *
PyXSTable_put (PyXSTableObject *self, PyObject *args)
{
    int         key;
    PyObject *  ob;
    PyXSObject *py_xs;

    if (!PyArg_ParseTuple (args, "iO", &key, &ob))
        return NULL;

    if (!PyObject_TypeCheck (ob, &PyXSType)) {
        PyErr_SetString (PyExc_RuntimeError, "xs must be a cross section");
        return NULL;
    }

    py_xs = (PyXSObject *) ob;

    xstable_put (self->xs_table, key, py_xs->xs);

    Py_INCREF (Py_None);
    return Py_None;
}

PyDoc_STRVAR (xstable_get__doc__,
              "get($self, key, /)\n"
              "--\n"
              "\n"
              "Gets a cross section from this reference table\n\n"
              "If no cross section is stored in the value of `key`, this\n"
              "method returns None.\n\n"
              "Parameters\n"
              "----------\n"
              "key : int\n"
              "    Key to store `xs`\n"
              "xs : :class:`pantherapy.panthera.CrossSection`\n"
              "    Cross section to store");

static PyObject *
PyXSTable_get (PyXSTableObject *self, PyObject *args)
{
    int          key;
    CrossSection xs;
    PyXSObject * py_xs;

    if (!PyArg_ParseTuple (args, "i", &key))
        return NULL;

    xs = xstable_get (self->xs_table, key);
    if (xs) {
        py_xs     = (PyXSObject *) PyXS_new (&PyXSType, NULL, NULL);
        py_xs->xs = xs;
        return py_xs;
    } else {
        Py_INCREF (Py_None);
        return Py_None;
    }
}

PyDoc_STRVAR (xstable_put__doc__,
              "put($self, key, xs, /)\n"
              "--\n"
              "\n"
              "Puts a cross section in this reference table\n\n"
              "If `key` already exists in this table, then the `xs` "
              "referenced by `key` will be overwritten\n\n"
              "Parameters\n"
              "----------\n"
              "key : int\n"
              "    Key to store `xs`\n"
              "xs : :class:`pantherapy.panthera.CrossSection`\n"
              "    Cross section to store");

static PyMethodDef PyXSTable_methods[] = {
    { "put", (PyCFunction) PyXSTable_put, METH_VARARGS, xstable_put__doc__ },
    { NULL }
};

static PyTypeObject PyXSTableType = {
    PyVarObject_HEAD_INIT (NULL, 0).tp_name = "pantherapy.panthera.XSTable",
    .tp_basicsize                           = sizeof (PyXSTableObject),
    .tp_itemsize                            = 0,
    .tp_flags                               = Py_TPFLAGS_DEFAULT,
    .tp_new                                 = PyXSTable_new,
    .tp_init                                = (initproc) PyXSTable_init,
    .tp_dealloc                             = (destructor) PyXSTable_dealloc,
    .tp_methods                             = PyXSTable_methods,
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
    if (PyType_Ready (&PyXSTableType) < 0)
        return NULL;

    m = PyModule_Create (&pantheramodule);
    if (m == NULL)
        return NULL;
    import_array ();

    Py_INCREF (&PyXSType);
    PyModule_AddObject (m, "CrossSection", (PyObject *) &PyXSType);
    PyModule_AddObject (m, "XSTable", (PyObject *) &PyXSTableType);
    return m;
}
