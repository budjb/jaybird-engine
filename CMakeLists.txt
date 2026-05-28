cmake_minimum_required(VERSION 4.2)
project(jaybird-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_COVERAGE OFF CACHE BOOL "Enable code coverage instrumentation")

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(BUILD_TESTING OFF)
else ()
  if (BUILD_TESTING STREQUAL "ON")
    add_compile_definitions(TESTING_ENABLED)
  endif ()

  include(FetchContent)
  FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.15.0)
  FetchContent_MakeAvailable(Catch2)

  if (ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(/EHsc -fprofile-instr-generate -fcoverage-mapping)
      add_link_options(-fprofile-instr-generate)
    endif ()
  endif ()

  include(CTest)
  include(Catch)
endif ()

add_subdirectory(core)
