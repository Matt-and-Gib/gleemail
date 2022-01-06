#Unit tests for queue.h

MESSAGE("inside queue cmakelists")

SET(testname "queue")
SET(testsource "ut_queue.cpp")

add_executable(${testname} ${testsource})
target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)

add_test(queue_no_children_count ${testname} 0)
add_test(queue_add_node ${testname} 1)
add_test(queue_one_child_count ${testname} 2)
add_test(queue_multiple_children_count ${testname} 3)
add_test(queue_enqueue_dequeue_order ${testname} 4)
add_test(queue_dequeue_empty ${testname} 5)
add_test(queue_dequeue_last ${testname} 6)
add_test(queue_dequeue_not_last ${testname} 7)
add_test(queue_enqueue_nullptr ${testname} 8)
add_test(queue_find_nonexistant ${testname} 9)
add_test(queue_find_match ${testname} 10)
add_test(queue_find_from_empty ${testname} 11)

#add_test(success_test ${testname} 0)
#set_tests_properties(success_test PROPERTIES PASS_REGULAR_EXPRESSION "SUCCESS")

#add_test(fail_test ${testname} 1)
#set_tests_properties(fail_test PROPERTIES WILL_FAIL TRUE)

#add_test(fail_test_output ${testname} 1)
#set_tests_properties(fail_test_output PROPERTIES PASS_REGULAR_EXPRESSION "FAIL")