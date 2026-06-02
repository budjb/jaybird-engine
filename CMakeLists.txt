cmake_minimum_required(VERSION 4.2)
project(jaybird-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build and enable tests" ON)
option(ENABLE_COVERAGE "Enable LLVM code coverage instrumentation" OFF)

include(cmake/CPM.cmake)
CPMAddPackage("gh:TheLartians/Format.cmake@1.8.2")

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(BUILD_TESTING OFF)
elseif (BUILD_TESTING)
  enable_testing()

  add_compile_definitions(TESTING_ENABLED)

  include(FetchContent)
  FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.15.0)
  FetchContent_MakeAvailable(Catch2)

  include(Catch)

  if (ENABLE_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND MSVC)
      message(STATUS "Enabling LLVM source-based coverage for clang-cl")
      add_compile_options(/EHsc -fprofile-instr-generate -fcoverage-mapping)
      add_link_options(-fprofile-instr-generate)
    endif ()
  endif ()
endif ()

add_subdirectory(core)

