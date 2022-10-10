cmake_minimum_required(VERSION 3.14)
project(linkedList)


add_executable(list-test test-linkedlist.cpp linkedlist.hpp)

target_link_libraries(
    list-test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(list-test)
