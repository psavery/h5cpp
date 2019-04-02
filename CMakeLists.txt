cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(h5cpp)

add_subdirectory(h5cpp)

option(BUILD_TESTS
  "Whether to compile the test suite as well as the main code."
  OFF)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif(BUILD_TESTS)
