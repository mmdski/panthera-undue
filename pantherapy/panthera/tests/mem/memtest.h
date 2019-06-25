#ifndef MEMTEST_INCLUDED
#define MEMTEST_INCLUDED
#include "testlib.h"
#include <math.h>
#include <panthera/cii/mem.h>
#include <panthera/crosssection.h>
#include <panthera/reach.h>
#include <panthera/standardstep.h>
#include <panthera/xstable.h>
#include <stddef.h>

extern void
test_xs_roughness (void);

extern void
test_xs_n_roughness (void);

extern void
test_xs_critical_depth (void);

extern void
test_reach (void);

extern void
test_xstable (void);

extern void
test_standardstep (void);
#endif
