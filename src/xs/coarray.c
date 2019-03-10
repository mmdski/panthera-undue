#include "coarray.h"
#include "cii/assert.h"
#include "cii/list.h"
#include "cii/mem.h"
#include <stddef.h>

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

    assert(a);
    int i;
    Coordinate_T c;
    for (i = 0; i < a->n; i++) {
        c = *(a->array + i);
        coord_free(c);
    }

    Mem_free((void *)a->array, __FILE__, __LINE__);

    FREE(a);
}

int coarray_eq(T a1, T a2) {

    Coordinate_T c1;
    Coordinate_T c2;

    int i;

    if (a1 == a2)
        return 1;

    if (a1->n != a2->n)
        return 0;

    for (i = 0; i < a1->n; i++) {
        c1 = *(a1->array + i);
        c2 = *(a2->array + i);
        if (!coord_eq(c1, c2))
            return 0;
    }

    return 1;
}

void _coarray_check(int n, double *x) {

    int i;
    for (i = 1; i < n; i++) {
        if (*(x + i - 1) > *(x + i))
            RAISE(coarray_new_Failed);
    }
}

int coarray_n(T a) {
    assert(a);
    return a->n;
}

Coordinate_T coarray_get(T a, int i) {
    assert(a);
    assert((int)i < a->n);
    return a->array[i];
}

T coarray_subarray_y(T a, double y) {

    assert(a);

    /* subarray to return */
    T sa;

    List_T list = NULL;
    int n; /* number of coordinates in the array */

    /* loop variables */
    int i;
    Coordinate_T c1     = NULL;
    Coordinate_T c2     = NULL;
    Coordinate_T c_last = NULL; /* keep track of the last coordinate added */

    /* check the first coordinate */
    c1 = *(a->array);

    /* if the y of the coordinate is less than or equal to y, add the
     * coordinate to the list
     */
    if (coord_y(c1) <= y) {
        c_last = coord_copy(c1);
        list   = List_push(list, c_last);
    }

    for (i = 1; i < a->n; i++) {

        c1 = *(a->array + i - 1);
        c2 = *(a->array + i);

        /* add an interpolated coordinate if coordinates change from
         * above to below or below to above the y value
         */
        if ((coord_y(c1) < y && y < coord_y(c2)) ||
            (y < coord_y(c1) && coord_y(c2) < y)) {
            c_last = coord_interp_y(c1, c2, y);
            list   = List_push(list, c_last);
        }

        /* add c2 if c2.y is at or below y */
        if (coord_y(c2) <= y) {
            c_last = coord_copy(c2);
            list   = List_push(list, c_last);
        }

        /* if the last coordinate added wasn't null and
         * both coordinates are above y, add a NULL coordinate
         */
        if (c_last != NULL && (i < (a->n) - 1) && (coord_y(c2) > y)) {
            c_last = NULL;
            list   = List_push(list, c_last);
        }
    }

    n = List_length(list);
    NEW(sa);
    sa->n = n;
    if (n == 0) {
        sa->array = NULL;
    } else {
        list      = List_reverse(list);
        sa->array = (Coordinate_T *)List_toArray(list, NULL);
    }

    if (list != NULL)
        List_free(&list);

    return sa;
}
