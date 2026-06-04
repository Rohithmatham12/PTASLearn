cmake_minimum_required(VERSION 3.20)
project(PTASLearn VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ptaslearn
  src/atpg.cpp
  src/branch_predictor.cpp
  src/cache_mesi.cpp
  src/cpu_model.cpp
  src/scan_chain.cpp
  src/x86_decoder.cpp
)

target_include_directories(ptaslearn PUBLIC include)

if(MSVC)
  target_compile_options(ptaslearn PRIVATE /W4)
else()
  target_compile_options(ptaslearn PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(ptaslearn_cli src/main.cpp)
target_link_libraries(ptaslearn_cli PRIVATE ptaslearn)

enable_testing()

add_executable(ptaslearn_tests
  tests/test_atpg.cpp
  tests/test_branch_predictor.cpp
  tests/test_cache_mesi.cpp
  tests/test_cpu_model.cpp
  tests/test_scan_chain.cpp
  tests/test_x86_decoder.cpp
  tests/test_main.cpp
)
target_link_libraries(ptaslearn_tests PRIVATE ptaslearn)
add_test(NAME ptaslearn_tests COMMAND ptaslearn_tests)
