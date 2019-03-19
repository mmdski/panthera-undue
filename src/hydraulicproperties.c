#include "panthera.h"
#include "cii/mem.h"

#define T HydraulicProps

struct T {
    double *properties;
};

T hp_new(void) {
    T hp;
    NEW(hp);
    hp->properties = Mem_calloc(N_HP, sizeof(double), __FILE__, __LINE__);
    return hp;
}

void hp_free(T hp) {
    Mem_free(hp->properties, __FILE__, __LINE__);
    FREE(hp);
}

double hp_get_property(T hp, hyd_prop prop) {
    return *(hp->properties + prop);
}

void hp_set_property(T hp, hyd_prop prop, double value) {
    *(hp->properties + prop) = value;
}
