#include "testlib.h"

bool
test_is_close(double calculated,
              double expected,
              double abs_tol,
              double rel_tol)
{
    double eps   = (abs_tol + rel_tol * fabs(expected));
    double delta = fabs(calculated - expected);
    if (delta <= eps) {
        return true;
    } else {
        fprintf(stderr, "test_is_close failed.\n");
        fprintf(stderr,
                "\t calculated=%f\t expected=%f\t eps=%f\t delta=%f\n",
                calculated,
                expected,
                eps,
                delta);
        return false;
    }
}
