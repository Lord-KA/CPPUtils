cmake_minimum_required(VERSION 3.14)

project(Deque)


add_executable(deque-test test-deque.cpp deque.hpp)

target_link_libraries(
    deque-test
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(deque-test)
