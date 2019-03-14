#include "testlib.h"
extern void add_coord_tests(void);
extern void add_subsection_tests(void);

void add_xs_tests(void) {
    add_coord_tests();
    add_subsection_tests();
}
