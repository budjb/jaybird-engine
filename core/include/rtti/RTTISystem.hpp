#pragma once

#include <functional>
#include <vector>

#include "Export.hpp"
#include "rtti/RTTIRegistry.hpp"

#ifdef TESTING_ENABLED
#define TEST_VISIBILITY public:
#else
#define TEST_VISIBILITY private:
#endif

namespace core::rtti {
/**
 * @brief Top-level manager for the RTTI type system, coordinating type registration and lifecycle.
 *
 * @c RTTISystem is a singleton that owns the @c RTTIRegistry and drives a two-phase type
 * registration process (declare then define) via callbacks registered before initialization.
 */
class JAYBIRD_API RTTISystem {
 public:
  /**
   * @brief This alias defines the callback type that is executed during initialization.
   */
  using CallbackFunction = std::function<void()>;

  /**
   * @brief Gets the singleton instance of the @code RTTISystem@endcode.
   *
   * @return A reference to the singleton instance of the @code RTTISystem@endcode.
   */
  static RTTISystem& get() noexcept;

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
   * @brief Gets a reference to the @c RTTIRegistry managed by the @code RTTISystem@endcode.
   *
   * @return A reference to the @c RTTIRegistry instance that is managed by the @code RTTISystem@endcode.
   */
  RTTIRegistry& registry() noexcept;

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
   * @brief Private constructor for the @c RTTISystem singleton.
   *
   * This constructor is private to prevent direct instantiation of the @c RTTISystem class, ensuring that only one
   * instance can exist and that it is accessed through the @c get() method.
   */
  TEST_VISIBILITY RTTISystem() noexcept;

 private:
  /**
   * @brief Registers built-in types with the RTTI system.
   */
  void registerBuiltInTypes() noexcept;

  /**
   * @brief This flag indicates whether the type system has been initialized.
   *
   * It ensures that initialization runs only once; subsequent calls to @c initialize() return @c false immediately.
   */
  bool m_initialized = false;

  /**
   * @brief This registry stores all type information managed by the @code RTTISystem@endcode.
   */
  RTTIRegistry m_registry;

  /**
   * @brief This collection stores callback functions that run during the declaration phase of initialization.
   */
  std::vector<CallbackFunction> m_declareFunctions;

  /**
   * @brief This collection stores callback functions that run during the definition phase of initialization.
   */
  std::vector<CallbackFunction> m_defineFunctions;
};
}  // namespace core::rtti
