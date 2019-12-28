#include <panthera/constants.h>

static double gravity            = 9.81;
static double manning_conversion = 1;

double
const_gravity(void)
{
    return gravity;
}

double
const_manning(void)
{
    return manning_conversion;
}

void
const_set_gravity(double g)
{
    gravity = g;
}

void
const_set_manning(double k)
{
    manning_conversion = k;
}
