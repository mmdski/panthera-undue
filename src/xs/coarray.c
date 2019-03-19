#include "coarray.h"
#include "cii/assert.h"
#include "cii/mem.h"
#include <stddef.h>
#include <stdio.h>

#define T CoArray

const Except_T coarray_n_coords_Error = {"Too few coordinates"};
const Except_T coarray_y_order_Error = {"Invalid y-value order"};

struct T {
    int length;        /* number of coordinates in this array */
    double min_z;      /* minimum z value in coordinate array */
    Coordinate *array; /* array of coordinates */
};

void check_y_coordinates(int n, Coordinate *array);

T coarray_new(int n, double *y, double *z) {

    if (n < 2)
        RAISE(coarray_n_coords_Error);

    if (y == NULL)
        RAISE(null_ptr_arg_Error);

    if (z == NULL)
        RAISE(null_ptr_arg_Error);

    T a;
    NEW(a);

    a->length = n;
    a->min_z  = INFINITY;
    a->array  = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        *(a->array + i) = coord_new(*(y + i), *(z + i));

        /* if on the first point or current coord y is less than minimum,
         * set minimum y to current point y
         */
        if (coord_z(*(a->array + i)) < a->min_z)
            a->min_z = coord_z(*(a->array + i));
    }

    check_y_coordinates(n, a->array);

    return a;
}

T coarray_copy(CoArray ca) {
    return coarray_from_array(ca->length, ca->array);
}

T coarray_from_array(int n, Coordinate *array) {

    int i;

    T a;

    check_y_coordinates(n, array);

    NEW(a);
    a->length = n;
    a->min_z  = INFINITY;
    a->array  = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        if (*(array + i) == NULL)
            *(a->array + i) = NULL;
        else {
            *(a->array + i) = coord_copy(*(array + i));
            if (coord_z(*(a->array + i)) < a->min_z)
                a->min_z = coord_z(*(a->array + i));
        }
    }

    return a;
}

T coarray_from_list(List_T list) {

    if (!list)
        RAISE(null_ptr_arg_Error);

    int i;
    int n;
    T a;

    Coordinate *tmp;

    n = List_length(list);

    NEW(a);
    a->length = n;
    a->min_z  = INFINITY;
    a->array  = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    tmp = (Coordinate *)List_toArray(list, NULL);

    check_y_coordinates(n, tmp);

    if (*(tmp) == NULL)
        *(a->array) = NULL;
    else {
        *(a->array) = coord_copy(*(tmp));
        a->min_z    = coord_z(*(tmp));
    }

    for (i = 1; i < n; i++) {

        /* if this coordinate is NULL, add NULL to the new array */
        if (*(tmp + i) == NULL)
            *(a->array + i) = NULL;

        /* otherwise, add a copy of the corrdinate to this array */
        else {
            *(a->array + i) = coord_copy(*(tmp + i));
            if (coord_z(*(a->array + i)) < a->min_z)
                a->min_z = coord_z(*(a->array + i));
        }
    }

    FREE(tmp);

    return a;
}

void coarray_free(T a) {

    if (!a)
        RAISE(null_ptr_arg_Error);

    int i;
    Coordinate c;
    for (i = 0; i < a->length; i++) {
        c = *(a->array + i);
        coord_free(c);
    }

    Mem_free((void *)a->array, __FILE__, __LINE__);

    FREE(a);
}

int coarray_eq(T a1, T a2) {

    Coordinate c1;
    Coordinate c2;

    int i;

    if (a1 == a2)
        return 1;

    if (a1->length != a2->length)
        return 0;

    for (i = 0; i < a1->length; i++) {
        c1 = *(a1->array + i);
        c2 = *(a2->array + i);
        if (!coord_eq(c1, c2))
            return 0;
    }

    return 1;
}

int coarray_length(T a) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    return a->length;
}

Coordinate coarray_get(T a, int i) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    assert((int)(0 <= i || i < a->length));
    Coordinate c = a->array[i];
    if (c)
        return coord_copy(a->array[i]);
    else
        return NULL;
}

double coarray_get_y(T a, int i) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    assert((int)(0 <= i || i < a->length));
    Coordinate c = a->array[i];
    if (c)
        return coord_y(c);
    else
        return NAN;
}

double coarray_get_z(T a, int i) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    assert((int)(0 <= i || i < a->length));
    Coordinate c = a->array[i];
    if (c)
        return coord_z(c);
    else
        return NAN;
}

double coarray_min_z(T a) { return a->min_z; }

T coarray_subarray_y(T a, double ylo, double yhi) {

    if (!a)
        RAISE(null_ptr_arg_Error);

    assert((int)(ylo < yhi));
    assert((int)(ylo != yhi));

    /* subarray to return */
    T sa;

    List_T list = NULL;

    int n; /* number of coordinates in the array */

    /* loop variables */
    int i;
    Coordinate c1 = NULL;
    Coordinate c2 = NULL;
    Coordinate c_interp;

    /* check the first coordinate, add it to the list if it's in the range */
    c1 = *(a->array);
    if (ylo <= coord_y(c1) && coord_y(c1) <= yhi)
        list = List_push(list, c1);

    for (i = 1; i < a->length; i++) {
        c1 = *(a->array + i - 1);
        c2 = *(a->array + i);

        /* add an interpolated point if ylo is between c1 and c2 */
        if (coord_y(c1) < ylo && ylo < coord_y(c2)) {
            c_interp = coord_interp_y(c1, c2, ylo);
            list     = List_push(list, c_interp);
        }

        /* add c2 if it is in the range */
        if (ylo <= coord_y(c2) && coord_y(c2) <= yhi)
            list = List_push(list, c2);

        /* add an interpolated point if yhi is between c1 and c2 */
        if (coord_y(c1) < yhi && yhi < coord_y(c2)) {
            c_interp = coord_interp_y(c1, c2, yhi);
            list     = List_push(list, c_interp);
        }
    }

    n = List_length(list);
    NEW(sa);
    sa->length = n;
    if (n == 0) {
        sa->array = NULL;
    } else {
        list      = List_reverse(list);
        sa->array = (Coordinate *)List_toArray(list, NULL);
    }

    if (list != NULL)
        List_free(&list);

    return sa;
}

T coarray_subarray_z(T a, double z) {

    if (!a)
        RAISE(null_ptr_arg_Error);

    /* subarray to return */
    T sa;

    List_T list = NULL;

    int n; /* number of coordinates in the array */

    /* loop variables */
    int i;
    Coordinate c1     = NULL;
    Coordinate c2     = NULL;
    Coordinate c_last = NULL; /* keep track of the last coordinate added */

    /* check the first coordinate */
    c1 = *(a->array);

    /* if the z of the coordinate is less than or equal to z, add the
     * coordinate to the list
     */
    if (coord_z(c1) <= z) {
        c_last = coord_copy(c1);
        list   = List_push(list, c_last);
    }

    for (i = 1; i < a->length; i++) {

        c1 = *(a->array + i - 1);
        c2 = *(a->array + i);

        /* add an interpolated coordinate if coordinates change from
         * above to below or below to above the z value
         */
        if ((coord_z(c1) < z && z < coord_z(c2)) ||
            (z < coord_z(c1) && coord_z(c2) < z)) {
            c_last = coord_interp_z(c1, c2, z);
            list   = List_push(list, c_last);
        }

        /* add c2 if c2.z is at or below z */
        if (coord_z(c2) <= z) {
            c_last = coord_copy(c2);
            list   = List_push(list, c_last);
        }

        /* if the last coordinate added wasn't NULL,
         * c2 isn't the last coordinate in the array,
         * and c2 is above z,
         * add a NULL spot in the
         */
        if (c_last != NULL && (i < (a->length) - 1) && (coord_z(c2) > z)) {
            c_last = NULL;
            list   = List_push(list, c_last);
        }
    }

    n = List_length(list);
    NEW(sa);
    sa->length = n;
    if (n == 0) {
        sa->array = NULL;
    } else {
        list      = List_reverse(list);
        sa->array = (Coordinate *)List_toArray(list, NULL);
    }

    if (list != NULL)
        List_free(&list);

    return sa;
}

void check_y_coordinates(int n, Coordinate *array) {

    assert(array);

    int i;

    Coordinate c;
    Coordinate last_c = NULL;

    for (i = 0; i < n; i++) {

        c = *(array + i);

        /* skip if either this or the last coordinate is null */
        if (c && last_c) {
            if (coord_y(c) < coord_y(last_c))
                RAISE(coarray_y_order_Error);
        }

        if (c)
            last_c = c;

    }
}
