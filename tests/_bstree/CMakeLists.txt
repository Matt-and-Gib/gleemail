#Unit tests for binarysearchtree.h

SET(testname "bstree")
SET(testsource "ut_bstree.cpp")

add_executable(${testname} ${testsource})
target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)

add_test(bstree_different_types ${testname} 0)

set_tests_properties(
	
	PROPERTIES LABELS bstree
)
