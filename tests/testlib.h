#include <cii/except.h>
#include <cii/mem.h>
#include <glib.h>
#include <math.h>
#include <panthera/exceptions.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern const Except_T no_Error;
extern bool test_is_close(double a, double b, double abs_tol, double rel_tol);
