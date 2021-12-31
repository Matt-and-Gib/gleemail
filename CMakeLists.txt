cmake_minimum_required(VERSION 3.17)
project (glEEmailTests)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic) # -Werror)

#file(GLOB LIB_SOURCES src/*.cpp) #This is okay because GitHub Actions doesn't cache our files (will be rebuilt every time)
#file(GLOB LIB_HEADERS src/include/*.h) #This is okay because GitHub Actions doesn't cache our files (will be rebuilt every time)
#add_library(glEEmailCode ${LIB_SOURCES} ${LIB_HEADERS})

add_subdirectory (tests)