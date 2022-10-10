cmake_minimum_required(VERSION 3.14)

project(Vector)

include_directories(./)

add_executable(vector-test test-vector.cpp vector.hpp)

target_link_libraries(
    vector-test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(vector-test)
