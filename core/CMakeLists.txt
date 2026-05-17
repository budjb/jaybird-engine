add_library(jaybird_core STATIC
        src/rtti/RTTI.cpp
        src/StringPool.cpp
        src/IString.cpp
        src/rtti/RTTIRegistry.cpp
        include/rtti/RTTIRegistry.hpp
        src/rtti/types/IntType.cpp
        include/rtti/types/IntType.hpp
        src/memory/MemoryPool.cpp
        include/memory/MemoryPool.hpp
        include/Common.hpp
        src/memory/Buffer.cpp
        include/memory/Buffer.hpp
        src/memory/Allocator.cpp
        include/memory/Allocator.hpp
        src/SpinLock.cpp
        include/SpinLock.hpp)

target_include_directories(jaybird_core PUBLIC "include")
