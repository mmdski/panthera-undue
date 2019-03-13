#include "panthera_test.h"

int test_is_close(double a, double b, double rel_tol) {
    double diff = a - b;
    return (fabs(diff) < rel_tol);
}
