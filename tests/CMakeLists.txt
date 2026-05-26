include(FetchContent)

FetchContent_Declare(
        Catch2
        URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.4.zip
)

FetchContent_MakeAvailable(Catch2)

add_executable(core-tests
        INameTests.cpp
        INamePoolTests.cpp
        FundamentalTypeTests.cpp
#        IntTypeTests.cpp
#        TypeRegistryTests.cpp
#        RttiIteratorTests.cpp
#        RttiReverseIteratorTests.cpp
#        RttiAutoRegistrationTests.cpp
#        RttiAutoRegistrationFundamentalsTest.cpp
)

target_include_directories(core-tests PRIVATE
        ${PROJECT_SOURCE_DIR}/core/include
        ${CMAKE_BINARY_DIR}/core/include
        ${PROJECT_SOURCE_DIR}/tests
)

target_link_libraries(core-tests PRIVATE
        Catch2::Catch2WithMain
        jaybird-engine-core
)

# Copy the DLL to the test directory so it can be found at runtime
add_custom_command(TARGET core-tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:jaybird-engine-core>
        $<TARGET_FILE_DIR:core-tests>
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(core-tests)
