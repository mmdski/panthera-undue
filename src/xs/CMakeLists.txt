add_library(coordinate STATIC coordinate.c)
target_link_libraries(coordinate PRIVATE mem)

add_library(coarray STATIC coarray.c)
target_link_libraries(coarray PRIVATE assert mem coordinate)
