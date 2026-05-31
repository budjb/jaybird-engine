#pragma once

#include "Function.hpp"

namespace core::rtti {
/**
 * @brief Base interface for reflected static functions.
 */
class IStaticFunction : public IFunction {
 public:
  /**
   * @brief Constructs a reflected static function descriptor.
   *
   * @param name This value is the reflected function name.
   * @param flags This value initializes the function flags.
   */
  explicit IStaticFunction(const std::string_view name, const FunctionFlags flags = {}) noexcept
      : IFunction(name, flags) {
    m_flags.isMember = false;
  }

  /**
   * @brief Destroys the reflected static function descriptor.
   */
  ~IStaticFunction() override = default;
};

/**
 * @brief Concrete reflected wrapper for static functions.
 *
 * @tparam F The static-function pointer type to wrap.
 */
template <FreeFunction F>
class TStaticFunction : public TFunction<F, IStaticFunction> {
 public:
  using traits = FunctionTraits<F>;

  /**
   * @brief Constructs a reflected wrapper for a static function.
   *
   * @tparam ArgNames These types provide one name per function argument.
   * @param name This value is the reflected function name.
   * @param function This value is the static function pointer to wrap.
   * @param argNames These values provide argument names in declaration order.
   */
  /**
   * @brief Constructs a reflected wrapper for a static function from a function name.
   *
   * @tparam ArgNames These types provide one name per function argument.
   * @param name This value is the reflected function name.
   * @param function This value is the static function pointer to wrap.
   * @param argNames These values provide argument names in declaration order.
   */
  template <typename... ArgNames>
    requires(FreeFunction<F> && sizeof...(ArgNames) == traits::numArgs &&
             (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit TStaticFunction(const std::string_view name, F function, ArgNames&&... argNames)
      : TFunction<F, IStaticFunction>(name, function, std::forward<ArgNames>(argNames)...) {}
};
}  // namespace core::rtti
