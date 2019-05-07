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
    CrossSection  xs;
} PyCrossSection;

static void
PyCrossSection_dealloc (PyCrossSection *self)
{
    Py_XDECREF (self->y);
    Py_XDECREF (self->z);
    if (self->xs)
        xs_free (self->xs);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyCrossSection_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCrossSection *self;
    self     = (PyCrossSection *) type->tp_alloc (type, 0);
    self->y  = NULL;
    self->z  = NULL;
    self->xs = NULL;
    return (PyObject *) self;
}

static int
PyCrossSection_init (PyCrossSection *self, PyObject *args, PyObject *kwds)
{
    int       y_size;
    int       z_size;
    int       n_roughness;
    double *  y_data;
    double *  z_data;
    double *  roughness_data;
    double *  z_roughness_data;
    PyObject *xs_roughness;
    PyObject *xs_z_roughness;

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

    xs_roughness =
        PyArray_FROM_OTF (roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (xs_roughness == NULL)
        return -1;

    /* if z_roughness is specified, get an array */
    if (z_roughness != NULL) {
        xs_z_roughness =
            PyArray_FROM_OTF (z_roughness, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
        if (xs_z_roughness == NULL)
            return -1;
    } else {
        xs_z_roughness = NULL;
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
    n_roughness = PyArray_SIZE ((PyArrayObject *) xs_roughness);
    if (n_roughness < 1) {
        PyErr_SetString (PyExc_ValueError,
                         "there must be at least one roughness value");
    }

    y_data         = (double *) PyArray_DATA ((PyArrayObject *) self->y);
    z_data         = (double *) PyArray_DATA ((PyArrayObject *) self->z);
    roughness_data = (double *) PyArray_DATA ((PyArrayObject *) xs_roughness);

    /* get a pointer to the z_roughness data if z_roughness was specified */
    if (z_roughness != NULL) {
        z_roughness_data =
            (double *) PyArray_DATA ((PyArrayObject *) xs_z_roughness);
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

static PyTypeObject CrossSectionType = {
    PyVarObject_HEAD_INIT (NULL, 0).tp_name =
        "pantherapy.panthera.CrossSection",
    .tp_doc       = "Cross section",
    .tp_basicsize = sizeof (PyCrossSection),
    .tp_itemsize  = 0,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_new       = PyCrossSection_new,
    .tp_init      = (initproc) PyCrossSection_init,
    .tp_dealloc   = (destructor) PyCrossSection_dealloc,
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
    if (PyType_Ready (&CrossSectionType) < 0)
        return NULL;

    m = PyModule_Create (&pantheramodule);
    if (m == NULL)
        return NULL;
    import_array ();

    Py_INCREF (&CrossSectionType);
    PyModule_AddObject (m, "CrossSection", (PyObject *) &CrossSectionType);
    return m;
}
