cmake_minimum_required(VERSION 3.17)
project (glEEmailTests)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic -Weffc++) # -Werror)

enable_testing()
add_compile_definitions(UNIT_TEST_MODE)

add_subdirectory (tests)