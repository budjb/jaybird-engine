add_executable(jaybird_engine
        src/main.cpp
)

target_link_libraries(jaybird_engine PRIVATE jaybird_core)
