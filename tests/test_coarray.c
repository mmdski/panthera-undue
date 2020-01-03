#include <glib.h>
#include <panthera/crosssection.h>
#include <stdbool.h>
#include <stdio.h>

void
test_coarray_new(void)
{
    int     n   = 4;
    double  y[] = { 0, 1, 2, 3 };
    double  z[] = { 0, 1, 2, 3 };
    CoArray ca  = coarray_new(n, y, z);

    g_assert_nonnull(ca);

    coarray_free(ca);
}

void
test_coarray_eq(void)
{
    int     n   = 4;
    double  y[] = { 2, 3, 4, 5 };
    double  z[] = { 0, 1, 2, 3 };
    CoArray ca1 = coarray_new(n, y, z);
    CoArray ca2 = coarray_new(n, y, z);
    CoArray ca3 = coarray_new(n, z, y);

    g_assert_true(coarray_eq(ca1, ca1) == 0);
    g_assert_true(coarray_eq(ca1, ca2) == 0);
    g_assert_false(coarray_eq(ca1, ca3) == 0);
    g_assert_false(coarray_eq(ca1, NULL) == 0);

    coarray_free(ca1);
    coarray_free(ca2);
    coarray_free(ca3);
}

void
test_coarray_copy(void)
{
    int     n       = 4;
    double  y[]     = { 2, 3, 4, 5 };
    double  z[]     = { 0, 1, 2, 3 };
    CoArray ca1     = coarray_new(n, y, z);
    CoArray ca_copy = coarray_copy(ca1);

    g_assert_true(coarray_eq(ca1, ca_copy) == 0);

    coarray_free(ca1);
    coarray_free(ca_copy);
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

    g_assert_true(coarray_eq(ca1, sa1) == 0);
    coarray_free(sa1);

    g_assert_true(coarray_eq(expected2, sa2) == 0);
    coarray_free(expected2);
    coarray_free(sa2);

    g_assert_true(coarray_eq(expected3, sa3) == 0);
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

    g_assert_true(coarray_eq(expected2, sa2) == 0);
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

    g_assert_true(coarray_eq(ca, sa) == 0);

    coarray_free(ca);
    coarray_free(sa);
}

void
test_coarray_subarray_trapezoid()
{
    int    n   = 6;
    double z[] = { 0, 0.25, 0.5, 1.5, 1.75, 2 };
    double y[] = { 1, 0.5, 0, 0, 0.5, 1 };

    CoArray ca = coarray_new(n, y, z);
    CoArray sa = coarray_subarray(ca, z[0], z[5]);

    g_assert_true(coarray_eq(ca, sa) == 0);

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

    g_assert_true(coarray_eq(expected1, sa1) == 0);
    coarray_free(sa1);
    coarray_free(expected1);

    g_assert_true(coarray_eq(expected2, sa2) == 0);
    coarray_free(sa2);
    coarray_free(expected2);

    coarray_free(ca);
}

void
test_coarray_subsection_multi_rectangle(void)
{
    int    n   = 5;
    double z[] = { 0, 0, 0.5, 1, 1 };
    double y[] = { 1, 0, 0, 0, 1 };

    CoArray ca  = coarray_new(n, y, z);
    CoArray sa1 = coarray_subarray(ca, 0, 0.25);
    CoArray sa2 = coarray_subarray(ca, 0.25, 0.75);
    CoArray sa3 = coarray_subarray(ca, 0.75, 1);

    coarray_free(ca);
    coarray_free(sa1);
    coarray_free(sa2);
    coarray_free(sa3);
}

void
test_coarray_length(void)
{
    int    n   = 4;
    double y[] = { 1.5, 1, 1, 1.5 };
    double z[] = { 0, 1, 2, 3 };

    CoArray ca     = coarray_new(n, y, z);
    int     length = coarray_length(ca);
    g_assert_true(length == n);

    coarray_free(ca);
}

void
test_coarray_get(void)
{
    int     n   = 4;
    double  y[] = { 1.5, 1, 1, 1.5 };
    double  z[] = { 0, 1, 2, 3 };
    CoArray ca  = coarray_new(n, y, z);

    for (int i = 0; i < n; i++) {

        Coordinate expected = coord_new(y[i], z[i]);
        Coordinate c        = coarray_get(ca, i);

        g_assert_true(coord_eq(expected, c) == 0);

        coord_free(expected);
        coord_free(c);
    }

    coarray_free(ca);
}

void
test_coarray_min_y(void)
{
    int     n   = 5;
    double  z[] = { 0, 0, 0.5, 1, 1 };
    double  y[] = { 21, 20, 20, 20, 21 };
    CoArray ca  = coarray_new(n, y, z);

    g_assert_true(coarray_min_y(ca) == 20);

    coarray_free(ca);
}

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/polonium-pollywog/coarray/new", test_coarray_new);
    g_test_add_func("/polonium-pollywog/coarray/eq", test_coarray_eq);
    g_test_add_func("/polonium-pollywog/coarray/copy", test_coarray_copy);
    g_test_add_func("/polonium-pollywog/coarray/subarray",
                    test_coarray_subarray);
    g_test_add_func("/polonium-pollywog/coarray/subarray_y",
                    test_coarray_subarray_y);
    g_test_add_func("/polonium-pollywog/coarray/subarray/equal z",
                    test_coarray_subarray_eq_z);
    g_test_add_func("/polonium-pollywog/coarray/subarray/trapezoid geometry",
                    test_coarray_subarray_trapezoid);
    g_test_add_func("/polonium-pollywog/coarray/subarray/double triangle",
                    test_coarray_subarray_double_triangle);
    g_test_add_func("/polonium-pollywog/coarray/subarray/"
                    "multi section rectangle",
                    test_coarray_subsection_multi_rectangle);
    g_test_add_func("/polonium-pollywog/coarray/length", test_coarray_length);
    g_test_add_func("/polonium-pollywog/coarray/get", test_coarray_get);
    g_test_add_func("/polonium-pollywog/coarray/min_y", test_coarray_min_y);

    return g_test_run();
}
