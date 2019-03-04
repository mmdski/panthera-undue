add_library(assert STATIC assert.c assert.h except.h)
add_library(except STATIC except.c assert.h except.h)
add_library(mem STATIC mem.c assert.h except.h mem.h)

target_link_libraries(assert PUBLIC except)
target_link_libraries(except PUBLIC assert)
target_link_libraries(mem PUBLIC assert except)
