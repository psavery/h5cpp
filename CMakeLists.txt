cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(h5cpp)

if(RUN_CODECOV)
  string(CONCAT CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -O0 "
                                "-fprofile-arcs -ftest-coverage")
  string(CONCAT CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -g -O0 "
                              "-fprofile-arcs -ftest-coverage")
  string(CONCAT CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} "
                                       "-fprofile-arcs -ftest-coverage")
endif(RUN_CODECOV)

add_subdirectory(h5cpp)

option(BUILD_TESTS
  "Whether to compile the test suite as well as the main code."
  OFF)

if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif(BUILD_TESTS)
