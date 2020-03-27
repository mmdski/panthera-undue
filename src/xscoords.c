#include "list.h"
#include "mem.h"
#include <assert.h>
#include <math.h>
#include <panthera/crosssection.h>
#include <stddef.h>

struct CoArray {
    int         length;      /* number of coordinates in this array */
    double      max_y;       /* maximum y in coarray */
    double      min_y;       /* minimum y in coarray */
    Coordinate *coordinates; /* array of coordinates */
};

static void
check_z_coordinates(int n, Coordinate *coordinates, const char *file, int line)
{
    assert(coordinates);

    for (int i = 1; i < n; i++) {
        assert((*(coordinates + i - 1))->z <= (*(coordinates + i))->z);
    }
}

CoArray
coarray_new(int n, double *y, double *z)
{
    assert(y);
    assert(z);

    double  max_y = -INFINITY;
    double  min_y = INFINITY;
    CoArray a;
    NEW(a);

    a->length      = n;
    a->coordinates = mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        *(a->coordinates + i) = coord_new(*(y + i), *(z + i));
        if (*(y + i) > max_y)
            max_y = *(y + i);
        if (*(y + i) < min_y)
            min_y = *(y + i);
    }
    a->max_y = max_y;
    a->min_y = min_y;

    check_z_coordinates(n, a->coordinates, __FILE__, __LINE__);

    return a;
}

static CoArray
coarray_from_array(int n, Coordinate *array)
{
    assert(array);

    CoArray     a;
    Coordinate *coordinates;
    double      max_y = -INFINITY;
    double      min_y = INFINITY;

    Coordinate c;

    if (n > 0) {
        coordinates = mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);
        for (int i = 0; i < n; i++) {
            c                  = *(array + i);
            *(coordinates + i) = c;
            if (c) {
                if (c->y > max_y)
                    max_y = c->y;
                if (c->y < min_y)
                    min_y = c->y;
            }
        }
    } else
        coordinates = NULL;

    NEW(a);
    a->coordinates = coordinates;
    a->length      = n;
    a->max_y       = max_y;
    a->min_y       = min_y;

    return a;
}

CoArray
coarray_copy(CoArray ca)
{
    assert(ca);

    int n = ca->length;

    Coordinate *coordinates =
        mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);
    CoArray copy;

    for (int i = 0; i < n; i++)
        coordinates[i] = coord_copy(ca->coordinates[i]);

    copy = coarray_from_array(n, coordinates);
    mem_free(coordinates, __FILE__, __LINE__);

    return copy;
}

CoArray
coarray_add_y(CoArray ca, double add_y)
{
    assert(ca);

    int     n = ca->length;
    double *y = mem_calloc(n, sizeof(double), __FILE__, __LINE__);
    double *z = mem_calloc(n, sizeof(double), __FILE__, __LINE__);

    int i;
    for (i = 0; i < n; i++) {
        y[i] = (*(ca->coordinates + i))->y + add_y;
        z[i] = (*(ca->coordinates + i))->z;
    }

    CoArray new_a = coarray_new(n, y, z);

    mem_free(y, __FILE__, __LINE__);
    mem_free(z, __FILE__, __LINE__);

    return new_a;
}

void
coarray_free(CoArray a)
{
    assert(a);

    int        i;
    Coordinate c;
    for (i = 0; i < a->length; i++) {
        c = *(a->coordinates + i);
        coord_free(c);
    }

    mem_free((void *) a->coordinates, __FILE__, __LINE__);

    FREE(a);
}

int
coarray_eq(CoArray a1, CoArray a2)
{

    Coordinate c1;
    Coordinate c2;

    int i;

    if (a1 == a2)
        return 0;

    /* check for either NULL */
    if (!a1 || !a2)
        return 1;

    if (a1->length != a2->length)
        return 1;

    for (i = 0; i < a1->length; i++) {
        c1 = *(a1->coordinates + i);
        c2 = *(a2->coordinates + i);
        if (coord_eq(c1, c2) != 0)
            return 1;
    }

    return 0;
}

double
coarray_max_y(CoArray a)
{
    assert(a);
    return a->max_y;
}

double
coarray_min_y(CoArray a)
{
    assert(a);
    return a->min_y;
}

int
coarray_length(CoArray a)
{
    assert(a);
    return a->length;
}

Coordinate
coarray_get(CoArray a, int i)
{
    assert(a);
    assert(0 <= i && i < a->length);

    if (a->coordinates[i])
        return coord_copy(a->coordinates[i]);
    else
        return NULL;
}

/* find the index of the coordinate with the greatest z value that's less than
 * or equal to zlo */
static int
find_zlo_idx(CoArray a, int lo, int hi, double zlo)
{
    if (lo == hi) {
        while (lo > 0 && a->coordinates[lo - 1]->z >= zlo) {
            lo--;
        }
        return a->coordinates[lo]->z <= zlo ? lo : -1;
    }

    int mid = (hi + lo) / 2;

    if (zlo < a->coordinates[mid]->z)
        return find_zlo_idx(a, lo, mid, zlo);

    int ret = find_zlo_idx(a, mid + 1, hi, zlo);

    return ret == -1 ? mid : ret;
}

static int
find_zhi_idx(CoArray a, int n, int lo, int hi, double zhi)
{
    if (lo == hi) {
        while (hi < n - 1 && a->coordinates[hi + 1]->z <= zhi) {
            hi++;
        }
        return a->coordinates[hi]->z >= zhi ? hi : -1;
    }

    int mid = (hi + lo) / 2;

    if (zhi <= a->coordinates[mid]->z)
        return find_zhi_idx(a, n, lo, mid, zhi);

    int ret = find_zhi_idx(a, n, mid + 1, hi, zhi);

    return ret == -1 ? mid : ret;
}

CoArray
coarray_subarray_y(CoArray a, double y)
{
    assert(a);

    int n = a->length;

    /* subarray to return */
    int         sa_length;
    List        list = list_new();
    CoArray     sa;
    Coordinate *coordinates;

    /* loop variables */
    Coordinate c1     = NULL;
    Coordinate c2     = NULL;
    Coordinate c_last = NULL; /* keep track of the last coordinate added */

    /* check the first coordinate */
    c1 = *(a->coordinates);

    /* if the y of the coordinate is less than or equal to y, add the
     * coordinate to the list
     */
    if (c1->y <= y) {
        c_last = coord_copy(c1);
        list_append(list, c_last);
    }

    for (int i = 1; i < n; i++) {

        c1 = *(a->coordinates + i - 1);
        c2 = *(a->coordinates + i);

        /* add an interpolated coordinate if coordinates change from
         * above to below or below to above the y value
         */
        if ((c1->y < y && y < c2->y) || (y < c1->y && c2->y < y)) {
            c_last = coord_interp_z(c1, c2, y);
            list_append(list, c_last);
        }

        /* add c2 if c2.z is at or below z */
        if (c2->y <= y) {
            c_last = coord_copy(c2);
            list_append(list, c_last);
        }

        /* if the last coordinate added wasn't NULL,
         * c2 isn't the last coordinate in the array,
         * and c2 is above y,
         * add a NULL spot in the
         */
        if (c_last != NULL && (i < n - 1) && (c2->y > y)) {
            c_last = NULL;
            list_append(list, c_last);
        }
    }

    sa_length = list_length(list);

    /* don't include the last coordinate if it was null */
    if (c_last == NULL)
        sa_length--;

    coordinates = (Coordinate *) list_to_array(list);
    list_free(list);
    sa = coarray_from_array(sa_length, coordinates);
    mem_free(coordinates, __FILE__, __LINE__);

    return sa;
}

CoArray
coarray_subarray(CoArray a, double zlo, double zhi)
{
    assert(a);
    assert(zhi > zlo);
    assert(a->coordinates[0]->z <= zlo);
    assert(zhi <= a->coordinates[a->length - 1]->z);

    double      eps = 1e-10;
    CoArray     sa;
    Coordinate  c0;
    Coordinate  c1;
    Coordinate *array =
        mem_calloc(a->length, sizeof(Coordinate), __FILE__, __LINE__);

    /* loop variables */
    int i  = find_zlo_idx(a, 0, a->length, zlo);
    int j  = 0;
    int hi = find_zhi_idx(a, a->length, 0, a->length, zhi);

    c0 = a->coordinates[i];
    c1 = a->coordinates[i + 1];

    if (fabs(c1->z - c0->z) <= eps)
        array[j++] = coord_copy(c0);
    else
        array[j++] = coord_interp_y(c0, c1, zlo);

    while (++i < hi) {
        array[j++] = coord_copy(a->coordinates[i]);
    }

    c0 = a->coordinates[i - 1];
    c1 = a->coordinates[i];
    if (fabs(c1->z - c0->z) <= eps)
        array[j++] = coord_copy(c1);
    else
        array[j++] = coord_interp_y(c0, c1, zhi);

    sa = coarray_from_array(j, array);
    mem_free(array, __FILE__, __LINE__);

    return sa;
}
