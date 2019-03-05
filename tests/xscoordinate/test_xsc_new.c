#include "panthera/xscoordinate.h"

int main() {
    double x = 0;
    double y = 0;

    int result = 0;

    XSCoordinate_T c = xsc_new(x, y);

    if (c == 0)
        result = -1;
    if (xsc_x(c) != x)
        result = -1;
    if (xsc_y(c) != y)
        result = -1;
    xsc_free(c);

    return result;
}
