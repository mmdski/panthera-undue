set(SOURCES test_xscoordinate.c)
add_library(test_xscoordinate STATIC test_xscoordinate.c)

set(STATION_TESTLIBS test_xscoordinate xscoordinate)
set(TESTLIBS ${STATION_TESTLIBS})

add_executable(test_panthera test_panthera.c ${SOURCES})
target_link_libraries(test_panthera -lcunit ${TESTLIBS})

add_test(test_panthera ${EXECUTABLE_OUTPUT_PATH}/test_panthera)
