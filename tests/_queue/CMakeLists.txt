#Unit tests for queue.h

MESSAGE("inside queue cmakelists")

file(GLOB TEST_FILES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR} *.cpp)
foreach(testsource ${TEST_FILES})
	MESSAGE("found project to build:")

	get_filename_component(testname ${testsource} NAME_WE)
	MESSAGE(${testname})

	add_executable(${testname} ${testsource})
	target_include_directories(${testname} PRIVATE "${PROJECT_SOURCE_DIR}/src/include")
	set_target_properties(${testname} PROPERTIES LINKER_LANGUAGE CXX)
endforeach()