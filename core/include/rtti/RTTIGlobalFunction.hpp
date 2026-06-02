#pragma once

#include "rtti/RTTIFunction.hpp"

namespace core::rtti {
/**
 * @brief Base interface for reflected global (non-member) functions.
 */
class JAYBIRD_API RTTIGlobalFunction : public RTTIFunction {
 public:
  /**
   * @brief Constructs a reflected global function descriptor.
   *
   * @param name This value is the reflected function name.
   * @param flags This value initializes the function flags.
   */
  explicit RTTIGlobalFunction(std::string_view name, FunctionFlags flags = {}) noexcept;

  /**
   * @brief Destroys the reflected global function descriptor.
   */
  ~RTTIGlobalFunction() override;
};
}  // namespace core::rtti
