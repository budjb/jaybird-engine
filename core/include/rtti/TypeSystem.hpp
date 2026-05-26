#pragma once
#include <functional>

#include "TypeRegistry.hpp"

namespace core::rtti {
/**
 * @brief Top-level manager for the RTTI type system, coordinating type registration and lifecycle.
 *
 * @c TypeSystem is a singleton that owns the @c TypeRegistry and drives a two-phase type
 * registration process (declare then define) via callbacks registered before initialization.
 */
class TypeSystem {
 public:
  /**
   * @brief Defines the type of function pointers used for callback functions in the RTTI system.
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
   * initialization are no-ops and return @code true@endcode.
   *
   * @return @c true if initialization succeeded or was already complete.
   */
  bool initialize();

  /**
   * @brief Gets a reference to the @c TypeRegistry managed by the @code TypeSystem@endcode.
   *
   * @return A reference to the @c TypeRegistry instance that is managed by the @code TypeSystem@endcode.
   */
  TypeRegistry& registry() noexcept;

  /**
   * @brief Adds a declaration callback function to the type system. This function will be called during the
   * registration process to declare a type to the RTTI system.
   *
   * @param function A function pointer to the callback function that should be added to the type system.
   */
  void addDeclareCallback(const CallbackFunction& function);

  /**
   * @brief Adds a definition callback function to the type system. This function will be called during the registration
   * process to define a type in the RTTI system.
   *
   * @param function A function pointer to the callback function that should be added to the type system.
   */
  void addDefineCallback(const CallbackFunction& function);

  /**
   * @brief Adds both declaration and definition callback functions to the type system.
   *
   * @param declare A function pointer to the declaration callback function that should be added to the type system.
   * @param define A function pointer to the definition callback function that should be added to the type system.
   */
  void addCallbacks(const CallbackFunction& declare, const CallbackFunction& define);

 private:
  /**
   * @brief Private constructor for the @c TypeSystem singleton.
   *
   * This constructor is private to prevent direct instantiation of the @c TypeSystem class, ensuring that only one
   * instance can exist and that it is accessed through the @c get() method.
   */
  TypeSystem() noexcept;

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
   * @brief Registers all types that have been automatically discovered.
   *
   * This method will be called during the initialization phase of the application to ensure that all types are properly
   * registered in the RTTI system.
   *
   * The registration process is performed in two phases. First, all declaration functions are called to declare the
   * types to the RTTI system, which allows the system to be aware of the existence of the types and their names without
   * needing any details about the types themselves. Afterward, all definition functions are called to provide the full
   * details of the types, such as their properties, functions, parent relationships, etc.
   *
   * This two-phase approach ensures that any inter-type relationships can be properly resolved during the definition
   * phase, as all types will have been declared and their names will be known to the RTTI system before any definitions
   * are processed.
   */
  void registerTypes();

  /**
   * @brief Collection of callback functions for declaring types in the RTTI system. Each function in this vector is
   * expected to take a non-const pointer to the @c TypeRegistry and perform the necessary declaration logic for a
   * specific type.
   */
  std::vector<CallbackFunction> m_declareFunctions;

  /**
   * @brief Collection of callback functions for defining types in the RTTI system. Each function in this vector is
   * expected to take a non-const pointer to the @c TypeRegistry and perform the necessary definition logic for a
   * specific type.
   */
  std::vector<CallbackFunction> m_defineFunctions;
};
}  // namespace core::rtti
