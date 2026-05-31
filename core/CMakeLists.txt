add_library(jaybird-engine-core SHARED
        src/INamePool.cpp
        src/IName.cpp
        src/rtti/RTTITypeRegistry.cpp
        include/rtti/RTTITypeRegistry.hpp
        src/SpinLock.cpp
        include/SpinLock.hpp
        include/rtti/RTTIType.hpp
        include/rtti/RTTITypeKind.hpp
        src/rtti/RTTIType.cpp
        include/rtti/RTTIContainerType.hpp
        src/rtti/RTTIContainerType.cpp
        src/rtti/RTTIArrayType.cpp
        include/rtti/RTTIArrayType.hpp
        include/rtti/RTTIClassType.hpp
        src/rtti/RTTIClassType.cpp
        include/rtti/RTTIFundamentalType.hpp
        src/rtti/RTTIFundamentalType.cpp
        include/rtti/RTTIIterator.hpp
        include/rtti/RTTITypeName.hpp
        include/Vector.hpp
        include/CString.hpp
        src/rtti/RTTITypeSystem.cpp
        include/rtti/RTTITypeSystem.hpp
        include/rtti/RTTIRefType.hpp
        include/rtti/RTTIWeakRefType.hpp
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
        include/rtti/RTTITType.hpp
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
          tests/rtti/RTTIFundamentalTypeTests.cpp
          tests/rtti/RTTIClassTypeTests.cpp
          tests/rtti/RTTIArrayTypeTests.cpp
          tests/rtti/RTTIIteratorTests.cpp
          tests/rtti/RTTIRefTypeTests.cpp
          tests/rtti/RTTIWeakRefTypeTests.cpp
          tests/rtti/RTTIINameTypeTests.cpp
          tests/rtti/RTTITypeSystemTests.cpp
          tests/rtti/RTTITypeRegistryTests.cpp
          tests/rtti/RTTITypeNameTests.cpp
          tests/rtti/ClassFunctionTests.cpp
          tests/rtti/GlobalFunctionTests.cpp
          tests/rtti/StaticFunctionTests.cpp
          tests/rtti/StackFrameTests.cpp
  )

  target_link_libraries(core-tests PRIVATE jaybird-engine-core Catch2::Catch2WithMain)
  catch_discover_tests(core-tests)
endif ()
