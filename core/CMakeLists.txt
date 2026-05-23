add_library(jaybird-engine-core STATIC
        src/INamePool.cpp
        src/IName.cpp
        src/rtti/TypeRegistry.cpp
        include/rtti/TypeRegistry.hpp
        src/rtti/types/IntType.cpp
        include/rtti/types/IntType.hpp
        src/SpinLock.cpp
        include/SpinLock.hpp
        include/rtti/IType.hpp
        include/rtti/TypeKind.hpp
        src/rtti/IType.cpp
        src/rtti/IContainerType.cpp
        include/rtti/IContainerType.hpp
        src/rtti/IArrayType.cpp
        include/rtti/IArrayType.hpp
        include/rtti/TType.hpp
        src/rtti/IClassType.cpp
        include/rtti/IClassType.hpp
        include/rtti/TArrayType.hpp
        include/rtti/Iterator.hpp
)

target_include_directories(jaybird-engine-core PUBLIC "include")
