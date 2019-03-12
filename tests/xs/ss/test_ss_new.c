#include "subsection.h"

int main() {
    int n                   = 10;
    double x[]              = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    double y[]              = {1, 0.75, 0.5, 0.25, 0, 0, 0.25, 0.5, 0.75, 1};
    double roughness        = 0.030;
    double activation_depth = 0;

    int test_result;

    Subsection ss = ss_new(n, x, y, roughness, activation_depth);

    if (ss == 0)
        test_result = 1;
    else
        test_result = 0;

    ss_free(ss);

    return test_result;
}
