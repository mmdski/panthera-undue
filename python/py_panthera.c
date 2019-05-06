#include <Python.h>
#include <structmember.h>

#define NPY_NO_DEPRECATED_API NPY_API_VERSION
#include <numpy/arrayobject.h>

#include <panthera/crosssection.h>

typedef struct {
    PyObject_HEAD    /* */
        PyObject *x; /* x values */
    PyObject *    y; /* y values */
} PyCrossSection;

static void
PyCrossSection_dealloc (PyCrossSection *self)
{
    Py_XDECREF (self->x);
    Py_TYPE (self)->tp_free ((PyObject *) self);
}

static PyObject *
PyCrossSection_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyCrossSection *self;
    self    = (PyCrossSection *) type->tp_alloc (type, 0);
    self->x = NULL;
    self->y = NULL;
    return (PyObject *) self;
}

static int
PyCrossSection_init (PyCrossSection *self, PyObject *args, PyObject *kwds)
{
    PyObject *x = NULL;
    PyObject *y = NULL;

    if (!PyArg_ParseTuple (args, "OO", &x, &y))
        return -1;

    self->x = PyArray_FROM_OTF (x, NPY_DOUBLE, NPY_ARRAY_IN_ARRAY);
    if (self->x == NULL)
        return -1;

    self->y = PyArray_FROM_OTF (y, NPY_DOUBLE, NPY_ARRAY_IN_ARRAY);
    if (self->y == NULL)
        return -1;

    return 0;
}

static PyMemberDef PyCrossSection_members[] = {
    { "x", T_OBJECT_EX, offsetof (PyCrossSection, x), 0, "x" },
    { "y", T_OBJECT_EX, offsetof (PyCrossSection, y), 0, "y" },
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
