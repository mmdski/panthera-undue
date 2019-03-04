add_subdirectory(cii)
add_library(xscoordinate STATIC xscoordinate.c)
target_link_libraries(xscoordinate PRIVATE mem)
