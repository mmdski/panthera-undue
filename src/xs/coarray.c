#include "coarray.h"
#include "cii/assert.h"
#include "cii/mem.h"

#define T CoArray_T

const Except_T coarray_new_Failed = {"coarray_new failed"};

struct T {
    Coordinate_T *array;
    int n;
};

void _coarray_check(int n, double *x);

T coarray_new(int n, double *x, double *y) {

    assert(x);
    assert(y);

    if (n < 2)
        RAISE(coarray_new_Failed);

    _coarray_check(n, x);

    T a;
    NEW(a);

    a->n = n;

    a->array = Mem_calloc(n, sizeof(Coordinate_T), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        *(a->array + i) = coord_new(*(x + i), *(y + i));
    }

    return a;
}

void coarray_free(T a) {
    int i;
    Coordinate_T c;
    for (i = 0; i < a->n; i++) {
        c = *(a->array + i);
        coord_free(c);
    }

    Mem_free((void *)a->array, __FILE__, __LINE__);

    FREE(a);
}

void _coarray_check(int n, double *x) {

    int i;
    for (i = 1; i < n; i++) {
        if (*(x + i - 1) > *(x + i))
            RAISE(coarray_new_Failed);
    }
}

int coarray_n(T a) { return a->n; }

void coarray_x(T a, double *x) {

    int i;
    Coordinate_T c;

    for (i = 0; i < a->n; i++) {
        c        = *(a->array + i);
        *(x + i) = coord_x(c);
    }
}

void coarray_y(T a, double *y) {

    int i;
    Coordinate_T c;

    for (i = 0; i < a->n; i++) {
        c        = *(a->array + i);
        *(y + i) = coord_y(c);
    }
}
