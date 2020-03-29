#include <glib.h>

#define CHL_ERROR chl_error_quark ()

extern GQuark chl_error_quark (void);

enum ChlError
{
  CHL_ERROR_ARG
};
