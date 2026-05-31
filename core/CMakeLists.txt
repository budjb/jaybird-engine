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
        include/Vector.hpp
        include/CString.hpp
        src/rtti/TypeSystem.cpp
        include/rtti/TypeSystem.hpp
        include/rtti/RefType.hpp
        include/rtti/WeakRefType.hpp
        src/types/Quaternion.cpp
        include/types/Quaternion.hpp
        src/types/EulerAngles.cpp
        include/types/EulerAngles.hpp
        src/types/Color.cpp
        include/types/Color.hpp
        include/rtti/ClassFunction.hpp
        include/rtti/Property.hpp
        include/rtti/StackFrame.hpp
        include/rtti/Function.hpp
        include/rtti/GlobalFunction.hpp
        include/rtti/StaticFunction.hpp
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
          tests/CStringTests.cpp
          tests/HashTests.cpp
          tests/SpinLockTests.cpp
          tests/types/QuaternionTests.cpp
          tests/types/EulerAnglesTests.cpp
          tests/types/ColorTests.cpp
          tests/rtti/FundamentalTypeTests.cpp
          tests/rtti/ClassTypeTests.cpp
          tests/rtti/ArrayTypeTests.cpp
          tests/rtti/IteratorTests.cpp
          tests/rtti/RefTypeTests.cpp
          tests/rtti/WeakRefTypeTests.cpp
          tests/rtti/INameTypeTests.cpp
          tests/rtti/TypeSystemTests.cpp
          tests/rtti/TypeRegistryTests.cpp
          tests/rtti/TypeNameTests.cpp
          tests/rtti/ClassFunctionTests.cpp
          tests/rtti/GlobalFunctionTests.cpp
          tests/rtti/StaticFunctionTests.cpp
          tests/rtti/StackFrameTests.cpp
  )

  target_link_libraries(core-tests PRIVATE jaybird-engine-core Catch2::Catch2WithMain)
  catch_discover_tests(core-tests)
endif ()
