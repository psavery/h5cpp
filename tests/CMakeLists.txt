
# find google test
find_package(GTest REQUIRED)

include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)
check_include_file_cxx("gtest/gtest.h" HAVE_GTEST_HPP)
if(HAVE_GTEST_HPP)
  check_cxx_symbol_exists(GTEST_HAS_PTHREAD "gtest/gtest.h" GTEST_HAS_PTHREAD)
  check_cxx_symbol_exists(GTEST_IS_THREADSAFE "gtest/gtest.h" GTEST_IS_THREADSAFE)
endif()

if(GTEST_HAS_PTHREAD)
  message(STATUS "GTest claims it has pthreads, we need to link to it.")
  find_package(Threads)
  set(EXTRA_LINK_LIB ${CMAKE_THREAD_LIBS_INIT})
else()
  set(EXTRA_LINK_LIB "")
endif()

option(RUN_CODE_COV
       "If this is true, then code coverage will be ran."
       OFF)

if(RUN_CODE_COV)
  string(CONCAT CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -g -O0 "
                                "-fprofile-arcs -ftest-coverage")
  string(CONCAT CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic -g -O0 "
                              "-fprofile-arcs -ftest-coverage")
  string(CONCAT CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} "
                                       "-fprofile-arcs -ftest-coverage")
endif(RUN_CODE_COV)

add_definitions(-DTESTDATADIR="${CMAKE_SOURCE_DIR}/tests/data/")

include_directories(${h5cpp_SOURCE_DIR})

add_subdirectory(reader)
