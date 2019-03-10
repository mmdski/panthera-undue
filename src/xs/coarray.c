#include "coarray.h"
#include "cii/assert.h"
#include "cii/mem.h"
#include <stddef.h>

#define T CoArray_T

const Except_T coarray_x_order_Error = {"Invalid x-value order"};

struct T {
    Coordinate_T *array;
    int n;
};

void coarray_check(int n, double *x);
void check_x_coordinates(int n, Coordinate_T *array);

T coarray_new(int n, double *x, double *y) {

    assert(x);
    assert(y);

    if (n < 2)
        RAISE(coarray_x_order_Error);

    coarray_check(n, x);

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

T coarray_from_array(int n, Coordinate_T *array) {

    int i;

    T a;

    check_x_coordinates(n, array);

    NEW(a);
    a->n     = n;
    a->array = Mem_calloc(n, sizeof(Coordinate_T), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        if (*(array + i) == NULL)
            *(a->array + i) = NULL;
        else
            *(a->array + i) = coord_copy(*(array + i));
    }

    return a;
}

T coarray_from_list(List_T list) {

    int i;
    int n;
    T a;

    Coordinate_T *tmp;

    n = List_length(list);

    NEW(a);
    a->n     = n;
    a->array = Mem_calloc(n, sizeof(Coordinate_T), __FILE__, __LINE__);

    tmp = (Coordinate_T *)List_toArray(list, NULL);

    check_x_coordinates(n, tmp);

    if (*(tmp) == NULL)
        *(a->array) = NULL;
    else
        *(a->array) = coord_copy(*(tmp));

    for (i = 1; i < n; i++) {

        /* if this coordinate is NULL, add NULL to the new array */
        if (*(tmp + i) == NULL)
            *(a->array + i) = NULL;

        /* if the previous x is less than this x, free memory and raise
         * error */
        // else if (coord_x(*(tmp + i)) < coord_x(*(tmp + i - 1))) {
        //     coarray_free(a);
        //     FREE(tmp);
        //     RAISE(coarray_x_order_Error);
        // }

        /* otherwise, add a copy of the corrdinate to this array */
        else
            *(a->array + i) = coord_copy(*(tmp + i));
    }

    FREE(tmp);

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

        /* if the last coordinate added wasn't NULL,
         * c2 isn't the last coordinate in the array,
         * and c2 is above y,
         * add a NULL spot in the
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

void coarray_check(int n, double *x) {

    int i;
    for (i = 1; i < n; i++) {
        if (*(x + i - 1) > *(x + i))
            RAISE(coarray_x_order_Error);
    }
}

void check_x_coordinates(int n, Coordinate_T *array) {

    int i;

    Coordinate_T last_c = NULL;
    if (*(array) != NULL)
        last_c = *(array);

    for (i = 1; i < n; i++) {

        /* skip if either this or the last coordinate is null */
        if (*(array + i) == NULL || last_c == NULL)
            continue;

        if (coord_x(*(array + i)) < coord_x(last_c))
            RAISE(coarray_x_order_Error);
    }
}
