cmake_minimum_required(VERSION 3.7)

project(scarlettxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-Wall -pedantic)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

set(CMAKE_BUILD_TYPE Debug)

link_directories(scarlettxx ${CMAKE_SOURCE_DIR}/libataxx/build/shared)

add_executable(
  scarlettxx
  main.cpp
  time.cpp
  uct.cpp
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)
target_link_libraries(scarlettxx Threads::Threads)

target_link_libraries(scarlettxx ataxx)
