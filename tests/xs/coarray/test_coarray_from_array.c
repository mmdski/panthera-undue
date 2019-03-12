#include "coarray_test.h"
#include "cii/mem.h"

int test_success();
int test_success_null();

int main() {
    int result = 0;

    if (test_success())
        result = 1;

    if (test_success_null())
        result = 1;

    return result;
}

int test_success() {

    printf("Starting test_success\n");

    int i;

    int test_result = 0;

    CoArray expected;
    CoArray result;

    Coordinate *array;

    int n      = 4;
    double x[] = {0, 0, 1, 1};
    double y[] = {1, 0, 0, 1};

    expected = coarray_new(n, x, y);

    array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        *(array + i) = coord_new(x[i], y[i]);
    }

    result = coarray_from_array(n, array);

    if (!coarray_eq(expected, result)) {
        printf("test_succeed failed\n");
        print_coarray_fail(expected, result);
        test_result = 1;
    }

    coarray_free(expected);
    coarray_free(result);

    for (i = 0; i < n; i++) {
        coord_free(*(array + i));
    }
    FREE(array);

    return test_result;
}

int test_success_null() {

    printf("Starting test_success_null\n");

    int i;

    int test_result = 0;
    int null_index  = 2;

    CoArray expected;
    CoArray result;

    Coordinate *array;

    int n      = 5;
    double x[] = {0, 0, 0, 1, 1};
    double y[] = {1, 0, 0, 0, 1};

    expected = coarray_new(n, x, y);

    array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        if (i == null_index)
            *(array + i) = NULL;
        else
            *(array + i) = coord_new(x[i], y[i]);
    }

    result = coarray_from_array(n, array);

    for (i = 0; i < n; i++) {
        if (i == null_index) {
            if (coarray_get(result, i) != NULL)
                test_result = 1;
        } else {
            if (!coord_eq(coarray_get(result, i), coarray_get(expected, i)))
                test_result = 1;
        }
    }

    if (test_result) {
        printf("test_succeed_null failed\n");
        print_coarray_fail(expected, result);
    }

    coarray_free(expected);
    coarray_free(result);

    for (i = 0; i < n; i++) {
        if (*(array + i) != NULL)
            coord_free(*(array + i));
    }
    FREE(array);

    return test_result;
}
