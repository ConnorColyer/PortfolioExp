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

# -------- Attribution library --------
add_library(attrib STATIC
  src/attrib/attribution.cpp
)

target_include_directories(attrib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(attrib PUBLIC
  graph
  scoring
  event
)

engine_apply_project_options(attrib)


# -------- Allocator library --------
add_library(alloc STATIC
  src/alloc/allocator.cpp
)

target_include_directories(alloc PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

engine_apply_project_options(alloc)

# -------- Event injection library --------
add_library(event STATIC
  src/event/injector.cpp
)

target_include_directories(event PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(event PUBLIC
  graph
)

engine_apply_project_options(event)

# -------- Simulation library --------

add_library(sim STATIC
  src/sim/simulation.cpp
)

target_include_directories(sim PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sim PUBLIC
  graph
  event
  scoring
  alloc
  risk
)

engine_apply_project_options(sim)



# -------- Backtest runner --------
add_library(backtest STATIC
  src/backtest/backtest.cpp
)

target_include_directories(backtest PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(backtest PUBLIC
  sim
  portfolio
)

engine_apply_project_options(backtest)


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
target_link_libraries(engine PRIVATE core graph scoring alloc event attrib sim portfolio backtest ingest risk)
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

add_executable(allocator_smoke
  tests/allocator_smoke.cpp
)

add_executable(injector_smoke
  tests/injector_smoke.cpp
)

target_link_libraries(injector_smoke PRIVATE
  event
)

engine_apply_project_options(injector_smoke)

add_executable(attribution_smoke
  tests/attribution_smoke.cpp
)

target_link_libraries(attribution_smoke PRIVATE
  attrib
)

engine_apply_project_options(attribution_smoke)

add_executable(simulation_smoke
  tests/simulation_smoke.cpp
)

target_link_libraries(simulation_smoke PRIVATE
  sim
)

engine_apply_project_options(simulation_smoke)

# -------- Portfolio accounting library --------
add_library(portfolio STATIC
  src/portfolio/portfolio.cpp
)

target_include_directories(portfolio PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(portfolio PUBLIC
  graph
)

engine_apply_project_options(portfolio)

add_executable(portfolio_smoke
  tests/portfolio_smoke.cpp
)

target_link_libraries(portfolio_smoke PRIVATE
  portfolio
)

engine_apply_project_options(portfolio_smoke)

# -------- Risk governor --------
add_library(risk STATIC
  src/risk/governor.cpp
)

target_include_directories(risk PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

engine_apply_project_options(risk)


# -------- Data ingestion --------
add_library(ingest STATIC
  src/ingest/validator.cpp
  src/ingest/arbiter.cpp
)

target_include_directories(ingest PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

engine_apply_project_options(ingest)


target_link_libraries(allocator_smoke PRIVATE alloc)
engine_apply_project_options(allocator_smoke)


target_link_libraries(scoring_smoke PRIVATE
  scoring
)

engine_apply_project_options(scoring_smoke)

add_executable(backtest_smoke
  tests/backtest_smoke.cpp
)

target_link_libraries(backtest_smoke PRIVATE
  backtest
)

engine_apply_project_options(backtest_smoke)

add_executable(ingest_smoke
  tests/ingest_smoke.cpp
)

target_link_libraries(ingest_smoke PRIVATE ingest)
engine_apply_project_options(ingest_smoke)

add_executable(arbiter_smoke
  tests/arbiter_smoke.cpp
)

target_link_libraries(arbiter_smoke PRIVATE ingest)
engine_apply_project_options(arbiter_smoke)


add_executable(governor_smoke
  tests/governor_smoke.cpp
)

target_link_libraries(governor_smoke PRIVATE risk)
engine_apply_project_options(governor_smoke)


# ------------------------- CTest wiring -------------------------
include(CTest)      # defines BUILD_TESTING option
enable_testing()

function(engine_register_smoke target_name)
  if (TARGET ${target_name})
    add_test(NAME ${target_name} COMMAND ${target_name})
  endif()
endfunction()

engine_register_smoke(graph_smoke)
engine_register_smoke(core_smoke)
engine_register_smoke(scoring_smoke)
engine_register_smoke(allocator_smoke)
engine_register_smoke(injector_smoke)
engine_register_smoke(attribution_smoke)
engine_register_smoke(simulation_smoke)
engine_register_smoke(portfolio_smoke)
engine_register_smoke(backtest_smoke)
engine_register_smoke(ingest_smoke)
engine_register_smoke(arbiter_smoke)
engine_register_smoke(governor_smoke)

