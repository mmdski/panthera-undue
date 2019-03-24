#include "testlib.h"

/* no exception placeholder */
const Except_T no_Error;

bool test_is_close(double a, double b, double abs_tol, double rel_tol) {
    if (fabs(a - b) <= (abs_tol + rel_tol * fabs(b))) {
        return true;
    } else {
        return false;
    }
}
