

add_executable(read_attributes_test read_attributes.cpp)
target_link_libraries(read_attributes_test h5cpp
                      ${GTEST_BOTH_LIBRARIES} ${EXTRA_LINK_LIB})

add_test(NAME read_attributes_test COMMAND read_attributes_test)
