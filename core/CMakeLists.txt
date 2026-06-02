add_library(jaybird-engine-core SHARED
        src/NamePool.cpp
        src/types/Name.cpp
        src/rtti/RTTIRegistry.cpp
        include/rtti/RTTIRegistry.hpp
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
        include/rtti/RTTIName.hpp
        include/Vector.hpp
        include/types/CString.hpp
        src/rtti/RTTISystem.cpp
        include/rtti/RTTISystem.hpp
        include/rtti/RTTIRefType.hpp
        include/rtti/RTTIWeakRefType.hpp
        src/types/Quaternion.cpp
        include/types/Quaternion.hpp
        src/types/EulerAngles.cpp
        include/types/EulerAngles.hpp
        src/types/Color.cpp
        include/types/Color.hpp
        include/rtti/RTTIClassFunction.hpp
        include/rtti/RTTIProperty.hpp
        include/rtti/RTTIStackFrame.hpp
        include/rtti/RTTIFunction.hpp
        src/rtti/RTTIFunction.cpp
        include/rtti/RTTIGlobalFunction.hpp
        include/rtti/RTTIStringType.hpp
        include/rtti/RTTITypeResolver.inl
        include/rtti/RTTITypeResolver.hpp
        src/rtti/RTTIFunction.cpp
        src/rtti/RTTIGlobalFunction.cpp
        src/rtti/RTTIClassFunction.cpp
        include/rtti/RTTITypedFunction.hpp
        src/types/Vector2.cpp
        include/types/Vector2.hpp
        src/types/Vector3.cpp
        include/types/Vector3.hpp
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
  message(STATUS "Building core tests")
  add_executable(core-tests
          tests/types/NameTests.cpp
          tests/NamePoolTests.cpp
          tests/types/CStringTests.cpp
          tests/HashTests.cpp
          tests/SpinLockTests.cpp
          tests/types/QuaternionTests.cpp
          tests/types/EulerAnglesTests.cpp
          tests/types/Vector2Tests.cpp
          tests/types/Vector3Tests.cpp
          tests/types/ColorTests.cpp
          tests/rtti/RTTIFundamentalTypeTests.cpp
          tests/rtti/RTTIClassTypeTests.cpp
          tests/rtti/RTTIArrayTypeTests.cpp
          tests/rtti/RTTIIteratorTests.cpp
          tests/rtti/RTTIRefTypeTests.cpp
          tests/rtti/RTTIWeakRefTypeTests.cpp
          tests/rtti/RTTINameTypeTests.cpp
          tests/rtti/RTTISystemTests.cpp
          tests/rtti/RTTIRegistryTests.cpp
          tests/rtti/RTTINameTests.cpp
          tests/rtti/RTTIClassFunctionTests.cpp
          tests/rtti/RTTIGlobalFunctionTests.cpp
          tests/rtti/RTTIStackFrameTests.cpp
          tests/rtti/RTTIStringTypeTests.cpp
  )

  target_link_libraries(core-tests PRIVATE jaybird-engine-core Catch2::Catch2WithMain)
  catch_discover_tests(core-tests)
endif ()
