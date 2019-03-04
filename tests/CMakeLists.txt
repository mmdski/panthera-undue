set(SOURCES test_xscoordinate.c test_xscarray.c)
add_library(test_xscoordinate STATIC test_xscoordinate.c)
add_library(test_xscarray STATIC test_xscarray.c)

set(XSC_TESTLIB test_xscoordinate xscoordinate)
set(XSCARRAY_TESTLIB test_xscarray xscarray)
set(TESTLIBS ${XSC_TESTLIB} ${XSCARRAY_TESTLIB})

add_executable(test_panthera test_panthera.c ${SOURCES})
target_link_libraries(test_panthera -lcunit ${TESTLIBS})

add_test(test_panthera ${EXECUTABLE_OUTPUT_PATH}/test_panthera)
