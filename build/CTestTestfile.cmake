# CMake generated Testfile for 
# Source directory: /Users/connorcolyer/Documents/CODE/PortfolioExp/engine
# Build directory: /Users/connorcolyer/Documents/CODE/PortfolioExp/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[graph_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/graph_smoke")
set_tests_properties([=[graph_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;339;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[core_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/core_smoke")
set_tests_properties([=[core_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;340;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[scoring_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/scoring_smoke")
set_tests_properties([=[scoring_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;341;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[allocator_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/allocator_smoke")
set_tests_properties([=[allocator_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;342;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[injector_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/injector_smoke")
set_tests_properties([=[injector_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;343;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[attribution_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/attribution_smoke")
set_tests_properties([=[attribution_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;344;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[simulation_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/simulation_smoke")
set_tests_properties([=[simulation_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;345;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[portfolio_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/portfolio_smoke")
set_tests_properties([=[portfolio_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;346;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[backtest_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/backtest_smoke")
set_tests_properties([=[backtest_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;347;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[ingest_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/ingest_smoke")
set_tests_properties([=[ingest_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;348;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[arbiter_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/arbiter_smoke")
set_tests_properties([=[arbiter_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;349;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
add_test([=[governor_smoke]=] "/Users/connorcolyer/Documents/CODE/PortfolioExp/build/governor_smoke")
set_tests_properties([=[governor_smoke]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;335;add_test;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;350;engine_register_smoke;/Users/connorcolyer/Documents/CODE/PortfolioExp/engine/CMakeLists.txt;0;")
subdirs("_deps/fmt-build")
subdirs("_deps/spdlog-build")
subdirs("_deps/catch2-build")
