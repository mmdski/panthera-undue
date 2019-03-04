add_subdirectory(cii)
add_library(station STATIC station.c)
target_link_libraries(station PRIVATE assert except mem)
