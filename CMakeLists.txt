cmake_minimum_required(VERSION 3.10)
project(scoulib LANGUAGES C)
set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99}")
include_directories("include")
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
