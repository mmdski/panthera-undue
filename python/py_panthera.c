#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#include <numpy/arrayobject.h>

#include <panthera/crosssection.h>
#include <panthera/exceptions.h>

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
PyXS_gety (PyXSObject *self, void *closure)
{
    return PyArray_NewCopy ((PyArrayObject *) self->y, NPY_CORDER);
}

static PyObject *
PyXS_getz (PyXSObject *self, void *closure)
{
    return PyArray_NewCopy ((PyArrayObject *) self->z, NPY_CORDER);
}

static PyObject *
PyXS_getroughness (PyXSObject *self, void *closure)
{
    return PyArray_NewCopy ((PyArrayObject *) self->roughness, NPY_CORDER);
}

static PyObject *
PyXS_getz_roughness (PyXSObject *self, void *closure)
{
    if (!self->z_roughness) {
        Py_INCREF (Py_None);
        return Py_None;
    } else
        return PyArray_NewCopy ((PyArrayObject *) self->z_roughness,
                                NPY_CORDER);
}

static PyGetSetDef PyXS_getsetters[] = {
    { "y", (getter) PyXS_gety, NULL, "y coordinate values", NULL },
    { "z", (getter) PyXS_getz, NULL, "z coordinate values", NULL },
    { "roughness",
      (getter) PyXS_getroughness,
      NULL,
      "roughness values",
      NULL },
    { "z_roughness",
      (getter) PyXS_getz_roughness,
      NULL,
      "z coordinates of roughness values",
      NULL },
    { NULL }
};

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
PyXS_wetted_perimeter (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_WETTED_PERIMETER);
}

static PyObject *
PyXS_top_width (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_TOP_WIDTH);
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
PyXS_conveyance (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CONVEYANCE);
}

static PyObject *
PyXS_velocity_coeff (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_VELOCITY_COEFF);
}

static PyObject *
PyXS_critical_flow (PyXSObject *self, PyObject *args)
{
    return PyXS_property (self, args, XS_CRITICAL_FLOW);
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

static PyMethodDef PyXS_methods[] = {
    { "area",
      (PyCFunction) PyXS_area,
      METH_VARARGS,
      "Returns the area for a depth" },
    { "wetted_perimeter",
      (PyCFunction) PyXS_wetted_perimeter,
      METH_VARARGS,
      "Returns the wetted perimeter for a depth" },
    { "top_width",
      (PyCFunction) PyXS_top_width,
      METH_VARARGS,
      "Returns the top width for a depth" },
    { "hydraulic_depth",
      (PyCFunction) PyXS_hydraulic_depth,
      METH_VARARGS,
      "Returns the hydraulic depth for a depth" },
    { "hydraulic_radius",
      (PyCFunction) PyXS_hydraulic_radius,
      METH_VARARGS,
      "Returns the hydraulic radius for a depth" },
    { "conveyance",
      (PyCFunction) PyXS_conveyance,
      METH_VARARGS,
      "Returns the conveyance for a depth" },
    { "velocity_coeff",
      (PyCFunction) PyXS_velocity_coeff,
      METH_VARARGS,
      "Returns the wetted perimeter for a depth" },
    { "critical_flow",
      (PyCFunction) PyXS_critical_flow,
      METH_VARARGS,
      "Returns the critical flow for a depth" },
    { "wp_array",
      (PyCFunction) PyXS_wp_array,
      METH_VARARGS,
      "Returns y, z ndarrays of wetted perimeter for a depth" },
    { "tw_array",
      (PyCFunction) PyXS_tw_array,
      METH_VARARGS,
      "Returns y, z ndarrays of top width for a depth" },
    { NULL }
};

static PyTypeObject PyXSType = {
    PyVarObject_HEAD_INIT (NULL, 0).tp_name =
        "pantherapy.panthera.CrossSection",
    .tp_doc       = "Cross section",
    .tp_basicsize = sizeof (PyXSObject),
    .tp_itemsize  = 0,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_new       = PyXS_new,
    .tp_init      = (initproc) PyXS_init,
    .tp_dealloc   = (destructor) PyXS_dealloc,
    .tp_methods   = PyXS_methods,
    .tp_getset    = PyXS_getsetters,
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

    m = PyModule_Create (&pantheramodule);
    if (m == NULL)
        return NULL;
    import_array ();

    Py_INCREF (&PyXSType);
    PyModule_AddObject (m, "CrossSection", (PyObject *) &PyXSType);
    return m;
}
