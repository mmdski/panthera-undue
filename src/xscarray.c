#include "cii/assert.h"
#include "cii/mem.h"
#include <panthera/xscarray.h>

#define T XSCArray_T

const Except_T xscarray_new_Failed = {"xscarray_new failed"};

struct T {
    XSCoordinate_T *array;
    int n;
};

void _xscarray_check_x(int n, double *x);

T xscarray_new(int n, double *x, double *y) {

    assert(x);
    assert(y);

    if (n < 2)
        RAISE(xscarray_new_Failed);

    _xscarray_check_x(n, x);

    T a;
    NEW(a);

    a->n = n;

    a->array = Mem_calloc(n, sizeof(XSCoordinate_T), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        *(a->array + i) = xsc_new(*(x + i), *(y + i));
    }

    return a;
}

void xscarray_free(T a) {
    int i;
    XSCoordinate_T xsc;
    for (i = 0; i < a->n; i++) {
        xsc = *(a->array + i);
        xsc_free(xsc);
    }

    Mem_free((void *)a->array, __FILE__, __LINE__);

    FREE(a);
}

void _xscarray_check_x(int n, double *x) {

    int i;
    for (i = 1; i < n; i++) {
        if (*(x + i - 1) > *(x + i))
            RAISE(xscarray_new_Failed);
    }
}

int xscarray_n(T a) { return a->n; }

void xscarray_x(T a, double *x) {

    int i;
    XSCoordinate_T xsc;

    for (i = 0; i < a->n; i++) {
        xsc      = *(a->array + i);
        *(x + i) = xsc_x(xsc);
    }
}

void xscarray_y(T a, double *y) {

    int i;
    XSCoordinate_T xsc;

    for (i = 0; i < a->n; i++) {
        xsc      = *(a->array + i);
        *(y + i) = xsc_y(xsc);
    }
}
