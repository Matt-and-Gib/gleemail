#Unit tests for queue.h

MESSAGE("inside queue cmakelists")

SET(testname "queue")
SET(testsource "ut_queue.cpp")

add_executable(${testname} ${testsource})
target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)

add_test(0_no_children_count_test ${testname} 0)

add_test(1_add_node ${testname} 1)

add_test(2_child_count_test ${testname} 2)

#add_test(success_test ${testname} 0)
#set_tests_properties(success_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

#add_test(fail_test ${testname} 1)
#set_tests_properties(fail_test PROPERTIES WILL_FAIL TRUE)

#add_test(fail_test_output ${testname} 1)
#set_tests_properties(fail_test_output PROPERTIES PASS_REGULAR_EXPRESSION "FAIL")