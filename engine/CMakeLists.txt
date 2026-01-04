cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(engine
  src/main.cpp
  src/graph/graph.cpp
)

target_include_directories(engine PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
