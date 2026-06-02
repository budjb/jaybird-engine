#pragma once

#include <string_view>

#include "RTTIFunction.hpp"

namespace core::rtti {
/**
 * @brief Represents a reflected member function of a class, including its name, flags, argument properties, return
 * type, and invocation logic.
 *
 * This class serves as the base for both native and scripted member functions, providing common metadata and an
 * interface for invocation. Native member functions can be directly invoked using C++ function pointers, while scripted
 * functions would implement the invocation logic to execute the corresponding script code.
 */
class JAYBIRD_API RTTIClassFunction : public RTTIFunction {
 public:
  /**
   * @brief Constructs an @c RTTIClassFunction with the given name and flags, initializing it as a member function.
   *
   * @param name The name of the function.
   * @param flags The flags indicating properties of the function.
   */
  explicit RTTIClassFunction(std::string_view name, FunctionFlags flags = {}) noexcept;

  /**
   * @brief Virtual destructor for @c RTTIClassFunction, allowing for proper cleanup of derived classes.
   */
  ~RTTIClassFunction() override;
};
}  // namespace core::rtti
