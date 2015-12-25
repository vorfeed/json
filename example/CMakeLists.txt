include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(video.pb.cc video.pb.h video.proto)
add_library(format_example video.pb.cc video.pb.h)

find_library(profiler
  NAMES profiler
  HINTS "/usr/local/lib"
)

target_link_libraries(
  format_example
  protobuf
  profiler
)

add_executable(json json_example.cpp)

add_executable(format format_example.cpp)
target_link_libraries(format format_example)

add_executable(rapidjson rapidjson_example.cpp)
target_link_libraries(rapidjson format_example)

add_executable(benchmark format_benchmark.cpp)
target_link_libraries(benchmark format_example)
