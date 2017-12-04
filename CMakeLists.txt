cmake_minimum_required(VERSION 2.8)

project(lab_asm_sse_lena)

set(CMAKE_CXX_FLAGS "-Wall -msse4")

set(SOURCE_EXE main.cpp)

add_library(im_lib im_lib.cpp)

add_executable(main ${SOURCE_EXE})

target_link_libraries(main im_lib)
