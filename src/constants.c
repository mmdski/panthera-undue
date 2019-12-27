#include <panthera/constants.h>

static double gravity = 9.81;

double
const_gravity(void)
{
    return gravity;
}

void
const_set_gravity(double g)
{
    gravity = g;
}
