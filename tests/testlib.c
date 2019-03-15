#include "testlib.h"

bool test_is_close(double a, double b, double abs_tol, double rel_tol) {
    if (fabs(a - b) <= (abs_tol + rel_tol * fabs(b))) {
        return true;
    } else {
        return false;
    }
}
