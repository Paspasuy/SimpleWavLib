cmake_minimum_required(VERSION 3.0)

project(WavLib)

SET(GCC_COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined,leak --std=c++20")
SET(GCC_LINK_FLAGS "-fsanitize=address,undefined,leak")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_LINK_FLAGS}")

add_library(WavFile SHARED src/WavFile.cpp)

add_executable(main src/main.cpp)

target_link_libraries(main WavFile)
