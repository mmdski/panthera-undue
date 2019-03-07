add_executable(test_coord_new test_coord_new.c)
target_link_libraries(test_coord_new coordinate)
add_test(test_coord_new ${EXECUTABLE_OUTPUT_PATH}/test_coord_new)
