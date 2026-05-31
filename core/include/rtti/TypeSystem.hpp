#pragma once

#include <functional>
#include <vector>

#include "Export.hpp"
#include "TypeRegistry.hpp"

#ifdef TESTING_ENABLED
#define TEST_VISIBILITY public:
#else
#define TEST_VISIBILITY private:
#endif

namespace core::rtti {
/**
 * @brief Top-level manager for the RTTI type system, coordinating type registration and lifecycle.
 *
 * @c TypeSystem is a singleton that owns the @c TypeRegistry and drives a two-phase type
 * registration process (declare then define) via callbacks registered before initialization.
 */
class JAYBIRD_API TypeSystem {
 public:
  /**
   * @brief This alias defines the callback type that is executed during initialization.
   */
  using CallbackFunction = std::function<void()>;

  /**
   * @brief Gets the singleton instance of the @code TypeSystem@endcode.
   *
   * @return A reference to the singleton instance of the @code TypeSystem@endcode.
   */
  static TypeSystem& get() noexcept;

  /**
   * @brief Initializes the type system by executing registered callbacks.
   *
   * Registration runs in two phases: first all declare callbacks, then all define callbacks. This two-phase approach
   * allows forward references between types to be resolved during the define phase. Subsequent calls after successful
   * initialization are no-ops and return @code false@endcode.
   *
   * @return This function returns @c true if initialization work ran during this call, and it returns @c false if the
   * system was already initialized.
   */
  bool initialize();

  /**
   * @brief Gets a reference to the @c TypeRegistry managed by the @code TypeSystem@endcode.
   *
   * @return A reference to the @c TypeRegistry instance that is managed by the @code TypeSystem@endcode.
   */
  TypeRegistry& registry() noexcept;

  /**
   * @brief Adds a declaration callback function to the type system.
   *
   * The callback runs in the declaration phase when @code initialize()@endcode is called.
   *
   * @param function This parameter provides the callback function that will be stored for declaration-phase execution.
   */
  void addDeclareCallback(const CallbackFunction& function);

  /**
   * @brief Adds a definition callback function to the type system.
   *
   * The callback runs in the definition phase after all declaration callbacks have completed.
   *
   * @param function This parameter provides the callback function that will be stored for definition-phase execution.
   */
  void addDefineCallback(const CallbackFunction& function);

  /**
   * @brief Adds both declaration and definition callback functions to the type system.
   *
   * @param declare This parameter provides the callback function that will run in the declaration phase.
   * @param define This parameter provides the callback function that will run in the definition phase.
   */
  void addCallbacks(const CallbackFunction& declare, const CallbackFunction& define);

  /**
   * @brief Private constructor for the @c TypeSystem singleton.
   *
   * This constructor is private to prevent direct instantiation of the @c TypeSystem class, ensuring that only one
   * instance can exist and that it is accessed through the @c get() method.
   */
  TEST_VISIBILITY TypeSystem() noexcept;

 private:
  /**
   * @brief Registers built-in types with the RTTI system.
   */
  void registerBuiltInTypes() noexcept;

  /**
   * @brief A flag indicating whether the type system has been initialized.
   *
   * This is used to ensure that the initialization process is only performed once, and that any subsequent calls to the
   * initialization method will simply return true without repeating the initialization logic.
   */
  bool m_initialized = false;

  /**
   * @brief The @c TypeRegistry instance that is managed by the @code TypeSystem@endcode.
   *
   * This registry is responsible for storing all type information for the RTTI system, and is accessed and modified
   * through the various registration functions and callback mechanisms provided by the @code TypeSystem@endcode.
   */
  TypeRegistry m_registry;

  /**
   * @brief Collection of callback functions for declaring types in the RTTI system.
   *
   * Each function in this collection takes no parameters and performs declaration-phase registration work.
   */
  std::vector<CallbackFunction> m_declareFunctions;

  /**
   * @brief Collection of callback functions for defining types in the RTTI system.
   *
   * Each function in this collection takes no parameters and performs definition-phase registration work.
   */
  std::vector<CallbackFunction> m_defineFunctions;
};

/**
 * @brief Helper struct that resolves a reflected type descriptor for a C++ type.
 *
 * This helper caches the registry lookup result after the first request so repeated
 * uses do not repeatedly query the type registry.
 *
 * @tparam T This type is the C++ type whose reflected descriptor is requested.
 */
template <typename T>
struct TypeResolver {
  /**
   * @brief Returns the reflected type descriptor for @c T.
   *
   * @return This function returns the reflected type descriptor for @c T, or @code nullptr@endcode when no descriptor
   * has been registered.
   */
  static IType* get() {
    static bool initialized = false;
    static IType* type = nullptr;

    if (!initialized) {
      type = TypeSystem::get().registry().getType(GetTypeName<T>());
      initialized = true;
    }

    return type;
  }

  /**
   * @brief Returns the reflected class descriptor for @c T when the resolved type is a class.
   *
   * @return This function returns the reflected class descriptor for @c T, or @code nullptr@endcode when the resolved
   * type is absent or not a class.
   */
  static IClassType* getClass() {
    if (auto* type = get(); type && type->kind() == TypeKind::CLASS) {
      return reinterpret_cast<IClassType*>(type);
    }

    return nullptr;
  }
};
}  // namespace core::rtti
