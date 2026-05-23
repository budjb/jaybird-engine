cmake_minimum_required(VERSION 4.2)
project(jaybird-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_subdirectory(core)

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()

