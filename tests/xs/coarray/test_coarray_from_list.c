#include "coarray_test.h"
#include "cii/except.h"
#include "cii/list.h"
#include "cii/mem.h"

int test_succeed();
int test_succeed_null();

int main() {

    int test_result = 0;

    if (test_succeed()) {
        test_result = 1;
    }

    if (test_succeed_null()) {
        test_result = 1;
    }

    return test_result;
}

/* this test is expected to successfully create an array from a list */
int test_succeed() {

    printf("Starting test_succeed\n");

    int test_result = 0;

    int i;

    CoArray expected;
    CoArray result;

    Coordinate c;
    List_T list = NULL;

    int n      = 4;
    double x[] = {0, 0, 1, 1};
    double y[] = {1, 0, 0, 1};

    expected = coarray_new(n, x, y);

    for (i = 0; i < n; i++) {
        c    = coord_new(x[i], y[i]);
        list = List_push(list, c);
    }

    list = List_reverse(list);

    result = coarray_from_list(list);

    if (!coarray_eq(expected, result)) {
        printf("test_succeed failed\n");
        print_coarray_fail(expected, result);
        test_result = 1;
    }

    /* free the coordinate arrays */
    coarray_free(expected);
    coarray_free(result);

    /* free the list */
    while (List_length(list) > 0) {
        list = List_pop(list, (void *)&c);
        coord_free(c);
    }
    List_free(&list);

    return test_result;
}

/* this test is expected to succesfull create an array from a list with
 * a NULL element
 */
int test_succeed_null() {

    printf("Starting test_succeed_null\n");

    int test_result = 0;

    int i;

    CoArray expected;
    Coordinate *array;

    CoArray result;

    Coordinate c;
    List_T list = NULL;

    int n      = 5;
    double x[] = {0, 0, 1, 1};
    double y[] = {1, 0, 0, 1};

    array = Mem_calloc(n, sizeof(Coordinate), __FILE__, __LINE__);

    for (i = 0; i < n; i++) {
        if (i == 2) {
            c            = NULL;
            *(array + i) = NULL;
        } else {
            c            = coord_new(x[i], y[i]);
            *(array + i) = coord_new(x[i], y[i]);
        }
        list = List_push(list, c);
    }

    expected = coarray_from_array(n, array);

    list   = List_reverse(list);
    result = coarray_from_list(list);

    if (!coarray_eq(expected, result)) {
        printf("test_succeed failed\n");
        print_coarray_fail(expected, result);
        test_result = 1;
    }

    /* free the coordinate arrays */
    coarray_free(expected);
    coarray_free(result);

    /* free the list */
    while (List_length(list) > 0) {
        list = List_pop(list, (void *)&c);
        coord_free(c);
    }
    List_free(&list);

    /* free the array */
    for (i = 0; i < n; i++) {
        coord_free(*(array + i));
    }

    FREE(array);

    return test_result;
}
