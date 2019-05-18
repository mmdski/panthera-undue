#include "memtest.h"

void
test_xstable (void)
{
    int          i;
    int          key;
    int *        key_array;
    CrossSection xs;

    int n_xs = 100;

    XSTable xstable = xstable_new ();

    /* fill the table with random keys */
    while (xstable_size (xstable) <= n_xs) {
        key = rand ();
        xs  = new_cross_section ();
        xstable_put (xstable, key, xs);
        xs_free (xs);
    }

    /* remove half of the cross sections from the table */
    n_xs = xstable_keys (xstable, &key_array);
    while (xstable_size (xstable) > n_xs / 2) {
        i = rand () % n_xs;
        xstable_delete (xstable, key_array[i]);
    }
    Mem_free (key_array, __FILE__, __LINE__);
    key_array = NULL;

    /* fill the table with twice the original amount */
    while (xstable_size (xstable) <= n_xs * 2) {
        key = rand ();
        xs  = new_cross_section ();
        xstable_put (xstable, key, xs);
        xs_free (xs);
    }

    /* remove all of the cross sections from the table */
    n_xs = xstable_keys (xstable, &key_array);
    while (xstable_size (xstable)) {
        i = rand () % n_xs;
        xstable_delete (xstable, key_array[i]);
    }
    Mem_free (key_array, __FILE__, __LINE__);
    key_array = NULL;

    /* fill the table with random keys */
    while (xstable_size (xstable) <= n_xs) {
        key = rand ();
        xs  = new_cross_section ();
        xstable_put (xstable, key, xs);
        xs_free (xs);
    }

    xstable_free (xstable);
}
