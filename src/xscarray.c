#include "cii/mem.h"
#include <panthera/xscarray.h>

#define T XSCArray_T

struct T {
    XSCoordinate_T *array;
    int n;
};

T xscarray_new(int n, double *x, double *y) {

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
