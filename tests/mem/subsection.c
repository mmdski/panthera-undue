#include <stddef.h>
#include <subsection.h>

void
test_subsection_new(void)
{
    int        n         = 4;
    double     roughness = 0.030;
    double     y[]       = { 0, 1, 2, 3 };
    double     z[]       = { 0, 1, 2, 3 };
    CoArray    ca        = coarray_new(n, y, z);
    Subsection ss        = subsection_new(ca, roughness, 0);

    coarray_free(ca);
    subsection_free(ss);

    ca = NULL;
    ss = NULL;

    ca = coarray_new(n, y, z);
    ss = subsection_new(ca, roughness, 0);

    subsection_free(ss);
    coarray_free(ca);
}

void
test_subsection_properties(void)
{
    int        n         = 4;
    double     roughness = 0.030;
    double     y[]       = { 0, 1, 2, 3 };
    double     z[]       = { 0, 1, 2, 3 };
    CoArray    ca        = coarray_new(n, y, z);
    Subsection ss        = subsection_new(ca, roughness, 0);

    CrossSectionProps xsp = subsection_properties(ss, 1.5);

    coarray_free(ca);
    subsection_free(ss);
    xsp_free(xsp);
}

void
test_double_triangle(void)
{
    int        n         = 9;
    double     roughness = 0.030;
    double     z[]       = { 0, 0.25, 0.5, 0.75, 1, 1.25, 1.5, 1.75, 2 };
    double     y[]       = { 1, 0.5, 0, 0.5, 1, 0.5, 0, 0.5, 1 };
    CoArray    ca        = coarray_new(n, y, z);
    Subsection ss        = subsection_new(ca, roughness, 0);

    CrossSectionProps xsp = subsection_properties(ss, 0.51);

    coarray_free(ca);
    subsection_free(ss);
    xsp_free(xsp);
}

void
test_subsection(void)
{
    test_subsection_new();
    test_subsection_properties();
    test_double_triangle();
}
