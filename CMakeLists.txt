cmake_minimum_required(VERSION 3.17)
project (glEEmailTests)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic) # -Werror)

MESSAGE("inside root cmakelists")

enable_testing()
add_subdirectory (tests)