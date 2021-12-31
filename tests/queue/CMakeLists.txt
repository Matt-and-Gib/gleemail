#Unit tests for queue.h

MESSAGE("inside queue cmakelists")

file(GLOB TEST_FILES ${CMAKE_CURRENT_LIST_DIR} *.cpp)
foreach(testsource ${TEST_FILES})
	get_filename_component(testname ${testsource} NAME_WE)
	add_executable(${testname} ${testsource})
	target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
	set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)
endforeach()