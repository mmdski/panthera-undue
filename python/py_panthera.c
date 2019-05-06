#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#include <numpy/arrayobject.h>

#include <panthera/coarray.h>
#include <panthera/crosssection.h>

typedef struct {
    PyObject_HEAD    /* */
        PyObject *y; /* y values */
    PyObject *    z; /* z values */
} PyCrossSection;

static void
PyCrossSection_dealloc (PyCrossSection *self)
{
    Py_XDECREF (self->y);
    Py_XDECREF (self->z);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyCrossSection_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCrossSection *self;
    self    = (PyCrossSection *) type->tp_alloc (type, 0);
    self->y = NULL;
    self->z = NULL;
    return (PyObject *) self;
}

static int
PyCrossSection_init (PyCrossSection *self, PyObject *args, PyObject *kwds)
{
    int     size;
    double *y_data;
    double *z_data;

    PyObject *y = NULL;
    PyObject *z = NULL;

    if (!PyArg_ParseTuple (args, "OO", &y, &z))
        return -1;

    self->y = PyArray_FROM_OTF (y, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (self->y == NULL)
        return -1;

    self->z = PyArray_FROM_OTF (z, NPY_DOUBLE, NPY_ARRAY_C_CONTIGUOUS);
    if (self->z == NULL)
        return -1;

    /* y and z must be 1D */
    if (PyArray_NDIM ((PyArrayObject *) self->y) != 1 ||
        PyArray_NDIM ((PyArrayObject *) self->z) != 1) {
        PyErr_SetString (PyExc_ValueError, "y and z must be 1D arrays");
        return -1;
    }

    /* y and z must be the same size */
    if (PyArray_SIZE ((PyArrayObject *) self->y) !=
        PyArray_SIZE ((PyArrayObject *) self->z)) {
        PyErr_SetString (PyExc_ValueError,
                         "the size of y and z must be equal");
        return -1;
    }

    size   = PyArray_SIZE ((PyArrayObject *) self->y);
    y_data = (double *) PyArray_DATA ((PyArrayObject *) self->y);
    z_data = (double *) PyArray_DATA ((PyArrayObject *) self->z);

    return 0;
}

static PyMemberDef PyCrossSection_members[] = {
    { "y", T_OBJECT_EX, offsetof (PyCrossSection, y), 0, "y" },
    { "z", T_OBJECT_EX, offsetof (PyCrossSection, z), 0, "z" },
    { NULL }
};

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
    .tp_members   = PyCrossSection_members,
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
