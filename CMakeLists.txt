cmake_minimum_required(VERSION 3.7)

project(scarlettxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

set(CMAKE_BUILD_TYPE Debug)

link_directories(scarlettxx ${CMAKE_SOURCE_DIR}/libataxx/build/shared)

add_executable(
  scarlettxx
  main.cpp
  time.cpp
  uct.cpp
)

target_link_libraries(scarlettxx ataxx)
