MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()



#file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true (${PROJECT_SOURCE_DIR}/tests) CMakeLists.txt) #This is okay because GitHub Actions doesn't cache our files (will be rebuilt every time)
foreach(testdirectory ${SUBDIRLIST})
	add_subdirectory(${testdirectory})
endforeach()
