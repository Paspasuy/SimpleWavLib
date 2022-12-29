cmake_minimum_required(VERSION 3.0)

project(WavLib)

add_library(WavFile SHARED src/WavFile.cpp)

add_executable(main src/main.cpp)

target_link_libraries(main WavFile)
