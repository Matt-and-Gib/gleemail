#Unit tests for queue.h

file(GLOB TEST_FILES ${CMAKE_CURRENT_LIST_DIR} *.h)
foreach(testsource ${TEST_FILES})
	get_filename_component(testname ${testsource} NAME_WE)
	add_executable(${testname} ${testsource})
	target_include_directories(${testname} ${PROJECT_SOURCE_DIR}/src/include)
foreach(testsource ${TEST_FILES})