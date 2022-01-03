#Unit tests for queue.h

MESSAGE("inside queue cmakelists")

SET(testname "queue")
SET(testsource "ut_queue.cpp")

add_executable(${testname} ${testsource})
target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)

add_test(usage_test ${testname})
set_tests_properties(usage_test PROPERTIES WILL_FAIL TRUE)

add_test(success_test ${testname} 0)
set_tests_properties(success_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

add_test(fail_test ${testname} 1)
set_tests_properties(fail_test PROPERTIES WILL_FAIL TRUE)

add_test(fail_test_output ${testname} 1)
set_tests_properties(fail_test_output PROPERTIES PASS_REGULAR_EXPRESSION "FAIL")