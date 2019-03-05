add_library(assert STATIC assert.c)
add_library(except STATIC except.c)
add_library(mem STATIC mem.c)

target_link_libraries(assert PUBLIC except)
target_link_libraries(except PUBLIC assert)
target_link_libraries(mem PUBLIC assert except)
