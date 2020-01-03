#include <assert.h>
#include <panthera/crosssection.h>
#include <stdio.h>

void
test_coarray_new(void)
{
    int     n   = 4;
    double  y[] = { 0, 1, 2, 3 };
    double  z[] = { 0, 1, 2, 3 };
    CoArray ca  = coarray_new(n, y, z);
    coarray_free(ca);
}

void
test_coarray_copy(void)
{
    int     n       = 4;
    double  y[]     = { 0, 1, 2, 3 };
    double  z[]     = { 0, 1, 2, 3 };
    CoArray ca      = coarray_new(n, y, z);
    CoArray ca_copy = coarray_copy(ca);
    coarray_free(ca);
    coarray_free(ca_copy);
}

void
test_coarray_get(void)
{
    int        n   = 4;
    double     y[] = { 0, 1, 2, 3 };
    double     z[] = { 0, 1, 2, 3 };
    CoArray    ca  = coarray_new(n, y, z);
    Coordinate c   = coarray_get(ca, 0);
    coord_free(c);
    coarray_free(ca);
}

void
test_coarray_subarray(void)
{
    int    n   = 4;
    double y[] = { 1.5, 1, 1, 1.5 };
    double z[] = { 0, 1, 2, 3 };

    CoArray ca1 = coarray_new(n, y, z);
    CoArray sa1 = coarray_subarray(ca1, z[0], z[3]);

    double  y_expected2[] = { 1.25, 1, 1, 1.25 };
    double  z_expected2[] = { 0.5, 1, 2, 2.5 };
    double  zlo2          = 0.5;
    double  zhi2          = 2.5;
    CoArray expected2     = coarray_new(n, y_expected2, z_expected2);
    CoArray sa2           = coarray_subarray(ca1, zlo2, zhi2);

    double  zlo3          = 2;
    double  y_expected3[] = { 1, 1.25 };
    double  z_expected3[] = { 2, 2.5 };
    CoArray expected3     = coarray_new(2, y_expected3, z_expected3);
    CoArray sa3           = coarray_subarray(ca1, zlo3, zhi2);

    coarray_free(sa1);

    coarray_free(expected2);
    coarray_free(sa2);

    coarray_free(expected3);
    coarray_free(sa3);

    coarray_free(ca1);
}

void
test_coarray_subarray_y(void)
{
    int    n   = 8;
    double y[] = { 90, 80, 78, 70, 71, 79, 81, 91 };
    double z[] = { 110, 120, 200, 210, 230, 240, 350, 360 };

    CoArray ca1 = coarray_new(n, y, z);

    int     n_expected    = 7;
    double  y_expected2[] = { 80.5, 80., 78., 70., 71., 79., 80.5 };
    double  z_expected2[] = { 119.5, 120., 200., 210., 230., 240., 322.5 };
    CoArray expected2     = coarray_new(n_expected, y_expected2, z_expected2);
    CoArray sa2           = coarray_subarray_y(ca1, 80.5);

    coarray_free(expected2);
    coarray_free(sa2);

    coarray_free(ca1);
}

void
test_coarray_subarray_eq_z(void)
{
    int    n   = 5;
    double z[] = { 0, 0, 0.5, 1, 1 };
    double y[] = { 1, 0, 0, 0, 1 };

    CoArray ca = coarray_new(n, y, z);
    CoArray sa = coarray_subarray(ca, z[0], z[4]);

    coarray_free(ca);
    coarray_free(sa);
}

void
test_coarray_subarray_double_triangle()
{
    int    n             = 9;
    double z[]           = { 0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 };
    double y[]           = { 1, 0.5, 0, 0.5, 1, 0.5, 0, 0.5, 1 };
    double z_expected1[] = { 0, 0.25, 0.5, 0.75, 1 };
    double y_expected1[] = { 1, 0.5, 0, 0.5, 1 };
    double z_expected2[] = { 1, 1.25, 1.5, 1.75, 2 };
    double y_expected2[] = { 1, 0.5, 0, 0.5, 1 };

    CoArray ca = coarray_new(n, y, z);

    CoArray expected1 = coarray_new(5, y_expected1, z_expected1);
    CoArray sa1       = coarray_subarray(ca, 0, 1);

    CoArray expected2 = coarray_new(5, y_expected2, z_expected2);
    CoArray sa2       = coarray_subarray(ca, 1, 2);

    coarray_free(sa1);
    coarray_free(expected1);

    coarray_free(sa2);
    coarray_free(expected2);

    coarray_free(ca);
}

void
test_coarray(void)
{
    test_coarray_new();
    test_coarray_copy();
    test_coarray_get();
    test_coarray_subarray();
    test_coarray_subarray_eq_z();
    test_coarray_subarray_double_triangle();
    test_coarray_subarray_y();
}
