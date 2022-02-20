#Unit tests for kvp.h

SET(testname "kvp")
SET(testsource "ut_kvp.cpp")

add_executable(${testname} ${testsource})
target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)

add_test(kvp_different_types ${testname} 0)
add_test(kvp_same_type ${testname} 1)
add_test(kvp_same_object ${testname} 2)
add_test(test_kvp_pointer_type ${testname} 3)

set_tests_properties(
	kvp_different_types
	kvp_same_type
	kvp_same_object
	test_kvp_pointer_type
	PROPERTIES LABELS kvp
)
