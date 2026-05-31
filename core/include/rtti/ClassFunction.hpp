#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

#include "Function.hpp"
#include "TypeSystem.hpp"

namespace core::rtti {
/**
 * @brief Represents a reflected member function of a class, including its name, flags, argument properties, return
 * type, and invocation logic.
 *
 * This class serves as the base for both native and scripted member functions, providing common metadata and an
 * interface for invocation. Native member functions can be directly invoked using C++ function pointers, while scripted
 * functions would implement the invocation logic to execute the corresponding script code.
 */
class IClassFunction : public IFunction {
 public:
  /**
   * @brief Constructs an @c IClassFunction with the given name and flags, initializing it as a member function.
   *
   * @param name The name of the function.
   * @param flags The flags indicating properties of the function.
   */
  explicit IClassFunction(const std::string_view name, const FunctionFlags flags = {}) noexcept
      : IFunction(name, flags) {
    m_flags.isMember = true;
  }

  /**
   * @brief Virtual destructor for @c IClassFunction, allowing for proper cleanup of derived classes.
   */
  ~IClassFunction() override = default;
};

/**
 * @brief A concrete implementation of @c IClassFunction that wraps a native C++ member function pointer, allowing it to
 * be invoked through the RTTI system.
 *
 * @tparam F The type of the member function pointer to wrap.
 */
template <MemberFunction F>
class TClassFunction : public TFunction<F, IClassFunction> {
 public:
  /**
   * @brief Traits extracted from the member function pointer type @c F, including the class type, return type, argument
   * types, and other properties.
   */
  using traits = FunctionTraits<F>;

  /**
   * @brief Constructs a @c TClassFunction by wrapping the given member function pointer and registering its argument
   * types and return type.
   *
   * @tparam ArgNames The types of the argument names, which must be convertible to @c std::string_view and match the
   * number of arguments in the function signature.
   *
   * @param name The name of the function as a string.
   * @param function The native member function pointer to wrap, which must satisfy the @c MemberFunction concept.
   * @param argNames The names of the arguments corresponding to the function's parameters, which must be provided in
   * the same order as the parameters in the function signature.
   * @throws std::runtime_error If any argument type or the return type (if applicable) cannot be found in the type
   * registry, or if the number of provided argument names does not match the number of arguments in the function
   * signature.
   */
  template <typename... ArgNames>
    requires(sizeof...(ArgNames) == traits::numArgs && (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit TClassFunction(const std::string_view name, F function, ArgNames&&... argNames)
      : TFunction<F, IClassFunction>(name, function, std::forward<ArgNames>(argNames)...) {}
};
}  // namespace core::rtti
