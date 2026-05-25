add_library(jaybird-engine-core SHARED
        src/INamePool.cpp
        src/IName.cpp
        src/rtti/TypeRegistry.cpp
        src/rtti/registration/TypeRegistrar.cpp
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
        include/rtti/registration/TypeRegistrar.hpp
        include/rtti/registration/TypeName.hpp
        include/rtti/registration/TypeDefinition.hpp
        include/rtti/registration/Macros.hpp
        include/rtti/registration/AutoRegistration.hpp
        include/rtti/registration/Specialization.hpp
        include/CString.hpp
        include/rtti/registration/Fundamentals.hpp
        src/rtti/registration/Fundamentals.cpp
)

include(GenerateExportHeader)
generate_export_header(jaybird-engine-core
    EXPORT_MACRO_NAME JAYBIRD_EXPORT
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/JaybirdCoreExport.hpp
    NO_EXPORT_MACRO_NAME JAYBIRD_NO_EXPORT
    DEFINE_NO_DEPRECATED
)

target_include_directories(jaybird-engine-core
    PUBLIC
        "include"
        ${CMAKE_CURRENT_BINARY_DIR}/include
)
