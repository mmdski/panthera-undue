add_executable(test_xsc_new test_xsc_new.c)
target_link_libraries(test_xsc_new xscoordinate)
add_test(test_xsc_new ${EXECUTABLE_OUTPUT_PATH}/test_xsc_new)
