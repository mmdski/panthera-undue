set(SOURCES test_station.c)
add_library(test_station STATIC test_station.c)

set(STATION_TESTLIBS test_station station)
set(TESTLIBS ${STATION_TESTLIBS})

add_executable(test_panthera test_panthera.c ${SOURCES})
target_link_libraries(test_panthera -lcunit ${TESTLIBS})

add_test(test_panthera ${EXECUTABLE_OUTPUT_PATH}/test_panthera)
