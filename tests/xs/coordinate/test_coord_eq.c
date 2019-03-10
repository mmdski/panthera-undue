#include "coordinate.h"

int main() {

    int result = 0;

    Coordinate_T c1 = coord_new(0, 0);
    Coordinate_T c2 = coord_new(0.1, 0.1);
    Coordinate_T c3 = coord_new(0.1, 0.1);

    if (!coord_eq(c1, c1))
        result = 1;

    if (!coord_eq(c2, c3))
        result = 1;

    if (coord_eq(c1, c2))
        result = 1;

    coord_free(c1);
    coord_free(c2);
    coord_free(c3);

    return result;
}
