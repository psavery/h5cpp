
enable_testing()

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

add_executable(test_read_file read_file.cpp)
target_link_libraries(test_read_file h5cpp)
add_test(test_read_file test_read_file)
