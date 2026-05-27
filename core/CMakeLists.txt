add_library(jaybird-engine-core SHARED
        src/INamePool.cpp
        src/IName.cpp
        src/rtti/TypeRegistry.cpp
        include/rtti/TypeRegistry.hpp
        src/SpinLock.cpp
        include/SpinLock.hpp
        include/rtti/IType.hpp
        include/rtti/TypeKind.hpp
        src/rtti/IType.cpp
        include/rtti/ContainerType.hpp
        src/rtti/ContainerType.cpp
        src/rtti/ArrayType.cpp
        include/rtti/ArrayType.hpp
        include/rtti/ClassType.hpp
        src/rtti/ClassType.cpp
        include/rtti/FundamentalType.hpp
        src/rtti/FundamentalType.cpp
        include/rtti/Iterator.hpp
        include/rtti/TypeName.hpp
        include/CString.hpp
        src/rtti/TypeSystem.cpp
        include/rtti/TypeSystem.hpp
        include/rtti/RefType.hpp
        include/rtti/WeakRefType.hpp
)

include(GenerateExportHeader)
generate_export_header(jaybird-engine-core
        EXPORT_MACRO_NAME JAYBIRD_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/Export.hpp
        NO_EXPORT_MACRO_NAME JAYBIRD_NO_EXPORT
        DEFINE_NO_DEPRECATED
)

target_include_directories(jaybird-engine-core
        PUBLIC
        "include"
        ${CMAKE_CURRENT_BINARY_DIR}/include
)

if (BUILD_TESTING)
  add_executable(core-tests
          tests/INameTests.cpp
          tests/INamePoolTests.cpp
          tests/rtti/FundamentalTypeTests.cpp
          tests/rtti/ClassTypeTests.cpp
          tests/rtti/ArrayTypeTests.cpp
          tests/rtti/IteratorTests.cpp
          include/rtti/RefType.hpp
          tests/rtti/RefTypeTests.cpp
          include/rtti/WeakRefType.hpp
          tests/rtti/WeakRefTypeTests.cpp
  )

  target_link_libraries(core-tests PRIVATE jaybird-engine-core Catch2::Catch2WithMain)
  catch_discover_tests(core-tests)
endif ()
