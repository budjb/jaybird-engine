cmake_minimum_required(VERSION 4.2)
project(jaybird-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.15.0)
FetchContent_MakeAvailable(Catch2)

set(ENABLE_COVERAGE OFF CACHE BOOL "Enable code coverage instrumentation")

if (ENABLE_COVERAGE)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(/EHsc -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
  endif ()
endif ()

include(CTest)
include(Catch)

add_subdirectory(core)
