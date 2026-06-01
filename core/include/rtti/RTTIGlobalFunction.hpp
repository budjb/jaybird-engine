#pragma once
#include "RTTIFunction.hpp"

namespace core::rtti {
/**
 * @brief Base interface for reflected global (non-member) functions.
 */
class RTTIGlobalFunction : public RTTIFunction {
 public:
  /**
   * @brief Constructs a reflected global function descriptor.
   *
   * @param name This value is the reflected function name.
   * @param flags This value initializes the function flags.
   */
  explicit RTTIGlobalFunction(const std::string_view name, const FunctionFlags flags = {}) noexcept
      : RTTIFunction(name, flags) {
    m_flags.isStatic = true;
  }

  /**
   * @brief Destroys the reflected global function descriptor.
   */
  ~RTTIGlobalFunction() override = default;
};

/**
 * @brief Concrete reflected wrapper for free functions.
 *
 * @tparam F The free-function pointer type to wrap.
 */
template <StaticFunction F>
class RTTIGlobalTFunction : public RTTITFunction<F, RTTIGlobalFunction> {
 public:
  using traits = FunctionTraits<F>;

  /**
   * @brief Constructs a reflected wrapper for a global function from a function name.
   *
   * @tparam ArgNames These types provide one name per function argument.
   * @param name This value is the reflected function name.
   * @param function This value is the global function pointer to wrap.
   * @param argNames These values provide argument names in declaration order.
   */
  template <typename... ArgNames>
    requires(StaticFunction<F> && sizeof...(ArgNames) == traits::numArgs &&
             (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit RTTIGlobalTFunction(std::string_view name, F function, ArgNames&&... argNames)
      : RTTITFunction<F, RTTIGlobalFunction>(name, function, std::forward<ArgNames>(argNames)...) {}
};
}  // namespace core::rtti
