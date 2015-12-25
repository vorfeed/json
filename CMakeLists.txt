cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_COMPILER "/usr/local/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++")

project(json)

add_definitions(
  -w
  -pthread -pipe
  -ggdb3 -O3
  -std=c++14 -D__GXX_EXPERIMENTAL_CXX11__
)

set(PROJ_ROOT ${PROJECT_SOURCE_DIR})

find_package(Protobuf REQUIRED)

include_directories(
  ${PROJ_ROOT}
  ${Protobuf_INCLUDE_DIRS}
)

add_subdirectory(example)
