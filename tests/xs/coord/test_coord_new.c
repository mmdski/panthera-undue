#include "coordinate.h"

int main() {
    double x = 0;
    double y = 0;

    int result = 0;

    Coordinate c = coord_new(x, y);

    if (c == 0)
        result = -1;
    if (coord_x(c) != x)
        result = -1;
    if (coord_y(c) != y)
        result = -1;
    coord_free(c);

    return result;
}
