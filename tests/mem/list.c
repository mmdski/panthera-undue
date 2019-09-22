#include <list.h>
#include <stdlib.h>

void
test_empty_list(void)
{
    List list = list_new();
    list_free(list);
}

void
test_simple_list(void)
{
    int n   = 5;
    int x[] = { 0, 1, 2, 3, 4 };

    List list = list_new();
    for (int i = 0; i < n; i++)
        list_append(list, &x[i]);

    list_free(list);
}

void
test_list_to_array(void)
{
    int  n   = 5;
    int  x[] = { 0, 1, 2, 3, 4 };
    int *array;

    List list = list_new();
    for (int i = 0; i < n; i++)
        list_append(list, (x + i));

    array = list_to_array(list);
    list_free(list);
    free(array);
}

void
test_list(void)
{
    test_empty_list();
    test_simple_list();
    test_list_to_array();
}
