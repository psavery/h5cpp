
set(tests
  ReadAttributes
  GetChildren
)

set(testSrcs "")
foreach(TestName ${tests})
  message(STATUS "Adding ${TestName} test.")
  string(TOLOWER ${TestName} testname)
  list(APPEND testSrcs ${testname}test.cpp)
endforeach()
message(STATUS "Test source files: ${testSrcs}")

# Add a single executable for all of our tests.
add_executable(ReaderTests ${testSrcs})
target_link_libraries(ReaderTests h5cpp
                      ${GTEST_BOTH_LIBRARIES} ${EXTRA_LINK_LIB})

# Now add all of the tests, using the gtest_filter argument so that only those
# cases are run in each test invocation.
foreach(TestName ${tests})
  add_test(NAME "Reader-${TestName}"
    COMMAND ReaderTests "--gtest_filter=${TestName}Test.*")
endforeach()
