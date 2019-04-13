#include <cii/list.h>
#include <cii/mem.h>
#include <panthera/coarray.h>
#include <panthera/exceptions.h>
#include <math.h>
#include <stddef.h>

/* coordinate */

/* Except_T raised when coordinate interpolation fails */
const Except_T coord_interp_Fail = {"Coordinate interpolation failed"};

struct Coordinate {
    double y; /* vertical coordinate */
    double z; /* lateral coordinate */
};

typedef struct Coordinate *Coordinate;

/* Creates and allocate space for a new Coordinate */
Coordinate coord_new(double y, double z) {

    Coordinate c;
    NEW(c);

    c->y = y;
    c->z = z;

    return c;
}

/* Makes a copy and returns a new Coordinate */
Coordinate coord_copy(Coordinate c) {
    assert(c);
    return coord_new((c->y), (c->z));
}

/* Frees space from a previously allocated Coordinate */
void coord_free(Coordinate c) {
    if (c)
        FREE(c);
}

/* Returns 1 if c1 and c2 are equal, 0 otherwise */
int coord_eq(Coordinate c1, Coordinate c2) {
    if (c1 == c2)
        return 1;
    /* either coordinate is NULL */
    if (!c1 || !c2)
        return 0;
    return (c1->y == c2->y && c1->z == c2->z);
}

/* Linearly interpolates Coordinate given a y value */
Coordinate coord_interp_y(Coordinate c1, Coordinate c2, double y) {

    assert(c1 && c2);

    /* raise exception if y is outside fo the range of c1->y and c2->y
      (no extrapolation) */
    if ((y < c1->y && y < c2->y) || (c1->y < y && c2->y < y))
        RAISE(coord_interp_Fail);

    double slope = (c2->z - c1->z) / (c2->y - c1->y);
    double z = slope * (y - c1->y) + c1->z;
    return coord_new(y, z);
}

/* Linearly interpolates Coordinate given a z value */
Coordinate coord_interp_z(Coordinate c1, Coordinate c2, double z) {

    assert(c1 && c2);

    /* raise exception if z is outside fo the range of c1->z and c2->z
      (no extrapolation) */
    if ((z < c1->z && z < c2->z) || (c1->z < z && c2->z < z))
        RAISE(coord_interp_Fail);

    double slope = (c2->y - c1->y) / (c2->z - c1->z);
    double y = slope * (z - c1->z) + c1->y;
    return coord_new(y, z);
}

/* x value from Coordinate */
double coord_y(Coordinate c) {
    assert(c);
    return c->y;
}

/* y value from Coordinate */
double coord_z(Coordinate c) {
    assert(c);
    return c->z;
}

/* coordinate array */

struct CoArray {
    int length;        /* number of coordinates in this array */
    double min_y;      /* minimum y value in coordinate array */
    Coordinate *array; /* array of coordinates */
};

void check_z_coordinates(int n, Coordinate *array, const char *file, int line);

CoArray coarray_new(int n, double *y, double *z) {

    if (n < 2)
        RAISE(coarray_n_coords_Error);

    if (y == NULL)
        RAISE(null_ptr_arg_Error);

    if (z == NULL)
        RAISE(null_ptr_arg_Error);

    CoArray a;
    NEW(a);

    a->length = n;
    a->min_y = INFINITY;
    a->array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        *(a->array + i) = coord_new(*(y + i), *(z + i));

        /* if on the first point or current coord y is less than minimum,
         * set minimum y to current point y
         */
        if ((*(a->array + i))->y < a->min_y)
            a->min_y = (*(a->array + i))->y;
    }

    check_z_coordinates(n, a->array, __FILE__, __LINE__);

    return a;
}

CoArray coarray_from_array(int n, Coordinate *array) {

    assert(array);

    int i;

    CoArray a;

    check_z_coordinates(n, array, __FILE__, __LINE__);

    NEW(a);
    a->length = n;
    a->min_y = INFINITY;
    a->array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        if (*(array + i) == NULL)
            *(a->array + i) = NULL;
        else {
            *(a->array + i) = coord_copy(*(array + i));
            if ((*(a->array + i))->y < a->min_y)
                a->min_y = (*(a->array + i))->y;
        }
    }

    return a;
}

CoArray coarray_from_list(List_T list) {

    assert(list);

    int i;
    int n;
    CoArray a;

    Coordinate *tmp;

    n = List_length(list);

    NEW(a);
    a->length = n;
    a->min_y = INFINITY;
    a->array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    tmp = (Coordinate *)List_toArray(list, NULL);

    check_z_coordinates(n, tmp, __FILE__, __LINE__);

    if (*(tmp) == NULL)
        *(a->array) = NULL;
    else {
        *(a->array) = coord_copy(*(tmp));
        a->min_y = (*(tmp))->y;
    }

    for (i = 1; i < n; i++) {

        /* if this coordinate is NULL, add NULL to the new array */
        if (*(tmp + i) == NULL)
            *(a->array + i) = NULL;

        /* otherwise, add a copy of the corrdinate to this array */
        else {
            *(a->array + i) = coord_copy(*(tmp + i));
            if ((*(a->array + i))->y < a->min_y)
                a->min_y = (*(a->array + i))->y;
        }
    }

    FREE(tmp);

    return a;
}

CoArray coarray_copy(CoArray ca) {
    if (!ca)
        RAISE(null_ptr_arg_Error);
    return coarray_from_array(ca->length, ca->array);
}

void coarray_free(CoArray a) {

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

int coarray_eq(CoArray a1, CoArray a2) {

    Coordinate c1;
    Coordinate c2;

    int i;

    if (a1 == a2)
        return 1;

    /* check for either NULL */
    if (!a1 || !a2)
        return 0;

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

int coarray_length(CoArray a) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    return a->length;
}

double coarray_get_y(CoArray a, int i) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    if (i < 0 || a->length <= i)
        RAISE(index_Error);

    Coordinate c = a->array[i];
    if (c)
        return c->y;
    else
        return NAN;
}

double coarray_get_z(CoArray a, int i) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    if (i < 0 || a->length <= i)
        RAISE(index_Error);

    Coordinate c = a->array[i];
    if (c)
        return c->z;
    else
        return NAN;
}

double coarray_min_y(CoArray a) {
    if (!a)
        RAISE(null_ptr_arg_Error);

    return a->min_y;
}

CoArray coarray_subarray_z(CoArray a, double zlo, double zhi) {

    if (!a)
        RAISE(null_ptr_arg_Error);

    if (zhi <= zlo)
        RAISE(value_arg_Error);

    /* subarray to return */
    CoArray sa;

    List_T list = NULL;

    int n; /* number of coordinates in the array */

    /* loop variables */
    int i;
    Coordinate c1 = NULL;
    Coordinate c2 = NULL;
    Coordinate c_interp;

    /* check the first coordinate, add it to the list if it's in the range */
    c1 = *(a->array);
    if (zlo <= c1->z && c1->z <= zhi)
        list = List_push(list, coord_copy(c1));

    for (i = 1; i < a->length; i++) {
        c1 = *(a->array + i - 1);
        c2 = *(a->array + i);

        /* add an interpolated point if ylo is between c1 and c2 */
        if (c1->z < zlo && zlo < c2->z) {
            c_interp = coord_interp_z(c1, c2, zlo);
            list = List_push(list, c_interp);
        }

        /* add c2 if it is in the range */
        if (zlo <= c2->z && c2->z <= zhi)
            list = List_push(list, coord_copy(c2));

        /* add an interpolated point if yhi is between c1 and c2 */
        if (c1->z < zhi && zhi < c2->z) {
            c_interp = coord_interp_z(c1, c2, zhi);
            list = List_push(list, c_interp);
        }
    }

    n = List_length(list);
    NEW(sa);
    sa->length = n;
    if (n == 0) {
        sa->array = NULL;
    } else {
        list = List_reverse(list);
        sa->array = (Coordinate *)List_toArray(list, NULL);
    }

    if (list != NULL)
        List_free(&list);

    return sa;
}

CoArray coarray_subarray_y(CoArray a, double y) {

    if (!a)
        RAISE(null_ptr_arg_Error);

    /* subarray to return */
    CoArray sa;

    List_T list = NULL;

    int n; /* number of coordinates in the array */

    /* loop variables */
    int i;
    Coordinate c1 = NULL;
    Coordinate c2 = NULL;
    Coordinate c_last = NULL; /* keep track of the last coordinate added */

    /* check the first coordinate */
    c1 = *(a->array);

    /* if the y of the coordinate is less than or equal to y, add the
     * coordinate to the list
     */
    if (c1->y <= y) {
        c_last = coord_copy(c1);
        list = List_push(list, c_last);
    }

    for (i = 1; i < a->length; i++) {

        c1 = *(a->array + i - 1);
        c2 = *(a->array + i);

        /* add an interpolated coordinate if coordinates change from
         * above to below or below to above the y value
         */
        if ((c1->y < y && y < c2->y) || (y < c1->y && c2->y < y)) {
            c_last = coord_interp_y(c1, c2, y);
            list = List_push(list, c_last);
        }

        /* add c2 if c2.z is at or below z */
        if (c2->y <= y) {
            c_last = coord_copy(c2);
            list = List_push(list, c_last);
        }

        /* if the last coordinate added wasn't NULL,
         * c2 isn't the last coordinate in the array,
         * and c2 is above y,
         * add a NULL spot in the
         */
        if (c_last != NULL && (i < (a->length) - 1) && (c2->y > y)) {
            c_last = NULL;
            list = List_push(list, c_last);
        }
    }

    n = List_length(list);
    NEW(sa);
    sa->length = n;
    if (n == 0) {
        sa->array = NULL;
    } else {
        list = List_reverse(list);
        sa->array = (Coordinate *)List_toArray(list, NULL);
    }

    if (list != NULL)
        List_free(&list);

    return sa;
}

CoArray coarray_add_y(CoArray ca, double add_y) {
    if (!ca)
        RAISE(null_ptr_arg_Error);

    int n = ca->length;
    double *y = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    double *z = Mem_calloc(n, sizeof(double), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        y[i] = (*(ca->array + i))->y + add_y;
        z[i] = (*(ca->array + i))->z;
    }

    CoArray new_a = coarray_new(n, y, z);

    Mem_free(y, __FILE__, __LINE__);
    Mem_free(z, __FILE__, __LINE__);

    return new_a;
}

void check_z_coordinates(int n, Coordinate *array, const char *file,
                         int line) {

    assert(array);

    int i;

    Coordinate c;
    Coordinate last_c = NULL;

    for (i = 0; i < n; i++) {

        c = *(array + i);

        /* skip if either this or the last coordinate is null */
        if (c && last_c) {
            if (c->z < last_c->z)
                Except_raise(&coarray_z_order_Error, file, line);
        }

        if (c)
            last_c = c;
    }
}
