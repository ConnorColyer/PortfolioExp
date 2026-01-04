cmake_minimum_required(VERSION 3.20)
project(engine LANGUAGES CXX)

# ---------- Global settings ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENGINE_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
option(ENGINE_ENABLE_SANITIZERS "Enable ASan/UBSan in Debug builds" ON)

# ---------- Dependencies (FetchContent) ----------
include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(catch2)

# ---------- Warnings / sanitizers ----------
function(engine_apply_project_options target_name)
  target_compile_features(${target_name} PRIVATE cxx_std_20)

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(${target_name} PRIVATE
      -Wall -Wextra -Wpedantic
      -Wshadow -Wconversion -Wsign-conversion
      -Wnull-dereference -Wdouble-promotion
      -Wformat=2
    )
    if (ENGINE_WARNINGS_AS_ERRORS)
      target_compile_options(${target_name} PRIVATE -Werror)
    endif()

    # Sanitizers: Debug only (and only if enabled)
    if (ENGINE_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined)
      target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
    endif()
  endif()
endfunction()

# ---------- Graph library ----------
add_library(graph STATIC
  src/graph/graph.cpp
)
target_include_directories(graph PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
engine_apply_project_options(graph)

# -------- Scoring library --------
add_library(scoring STATIC
  src/scoring/scorer.cpp
)

target_include_directories(scoring PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(scoring PUBLIC
  graph
  core
)

engine_apply_project_options(scoring)


# ---------- Core library (logging, utilities) ----------
add_library(core STATIC
  src/core/log.cpp
)
target_include_directories(core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(core PUBLIC spdlog::spdlog fmt::fmt)
engine_apply_project_options(core)

# ---------- Main engine executable ----------
add_executable(engine
  src/main.cpp
)
target_link_libraries(engine PRIVATE core graph scoring)
engine_apply_project_options(engine)

# ---------- Tests ----------
add_executable(graph_smoke
  tests/graph_smoke.cpp
)
target_link_libraries(graph_smoke PRIVATE core graph)
engine_apply_project_options(graph_smoke)

add_executable(core_smoke
  tests/core_smoke.cpp
)
target_link_libraries(core_smoke PRIVATE core Catch2::Catch2WithMain)
engine_apply_project_options(core_smoke)

add_executable(scoring_smoke
  tests/scoring_smoke.cpp
)

target_link_libraries(scoring_smoke PRIVATE
  scoring
)

engine_apply_project_options(scoring_smoke)

