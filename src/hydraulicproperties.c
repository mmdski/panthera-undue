#include "panthera/hydraulicproperties.h"
#include "panthera/panthera.h"

struct HydraulicProps {
    double *properties;
};

HydraulicProps hp_new(void) {
    HydraulicProps hp;
    NEW(hp);
    hp->properties = Mem_calloc(N_HP, sizeof(double), __FILE__, __LINE__);
    return hp;
}

void hp_free(HydraulicProps hp) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    Mem_free(hp->properties, __FILE__, __LINE__);
    FREE(hp);
}

double hp_get_property(HydraulicProps hp, hyd_prop prop) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    return *(hp->properties + prop);
}

void hp_set_property(HydraulicProps hp, hyd_prop prop, double value) {
    if (!hp)
        RAISE(null_ptr_arg_Error);
    *(hp->properties + prop) = value;
}
