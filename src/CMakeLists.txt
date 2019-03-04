add_subdirectory(cii)

add_library(xscoordinate STATIC xscoordinate.c)
target_link_libraries(xscoordinate PRIVATE mem)

add_library(xscarray STATIC xscarray.c)
target_link_libraries(xscarray PRIVATE mem xscoordinate)
