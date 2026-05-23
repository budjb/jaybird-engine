cmake_minimum_required(VERSION 4.2)
project(jaybird-engine LANGUAGES CXX)

set(ENABLE_COVERAGE OFF CACHE BOOL "Enable code coverage instrumentation")

if (ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(/EHsc -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
  endif ()
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_subdirectory(core)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()
