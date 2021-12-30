cmake_minimum_required(VERSION 3.17)
project (glEEmailTests)

file( GLOB LIB_SOURCES src/*.cpp )
file( GLOB LIB_HEADERS src/include/*.h )
add_library( YourLib ${LIB_SOURCES} ${LIB_HEADERS} )

add_subdirectory (tests)
