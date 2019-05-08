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
    self     = (PyXSObject *) type->tp_alloc (type, 0);
    self->y  = NULL;
    self->z  = NULL;
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
