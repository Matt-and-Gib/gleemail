#MACRO(SUBDIRLIST result curdir)
#  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
#  SET(dirlist "")
#  FOREACH(child ${children})
#    IF(IS_DIRECTORY ${curdir}/${child})
#      LIST(APPEND dirlist ${child})
#    ENDIF()
#  ENDFOREACH()
#  SET(${result} ${dirlist})
#ENDMACRO()


MESSAGE("inside tests cmakelists")

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/queue)


#SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})
#foreach(testdirectory ${SUBDIRLIST})
#	MESSAGE("found a child")
#	add_subdirectory(${testdirectory})
#endforeach()
