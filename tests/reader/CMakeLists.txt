

add_executable(test_read_file read_file.cpp)
target_link_libraries(test_read_file h5cpp
                      ${GTEST_BOTH_LIBRARIES} ${EXTRA_LINK_LIB})

add_test(NAME test_read_file COMMAND test_read_file)
