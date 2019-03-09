#include "coordinate.h"

int main() {

    int result        = 0;
    double expected_x = 0.5;
    double expected_y = 0.5;

    Coordinate_T c1 = coord_new(0, 0);
    Coordinate_T c2 = coord_new(1, 1);

    Coordinate_T c3 = coord_interp_y(c1, c2, 0.5);

    if (coord_x(c3) != expected_x)
        result = 1;

    if (coord_y(c3) != expected_y)
        result = 1;

    coord_free(c1);
    coord_free(c2);
    coord_free(c3);

    return result;
}
