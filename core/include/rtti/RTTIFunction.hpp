#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "rtti/RTTIArgument.hpp"
#include "rtti/RTTIStackFrame.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief Flags to indicate properties of a function.
 */
struct FunctionFlags {
  /**
   * @brief This flag indicates whether the function is a native C++ function that can be directly invoked, as opposed
   * to a scripted or interpreted function.
   */
  bool isNative : 1;

  /**
   * @brief This flag indicates whether the function is invocable without an instance pointer.
   *
   * It is set for free functions and static member functions, and cleared for non-static member functions.
   */
  bool isStatic : 1;
};

class RTTIFunction;

using RTTIFunctionPtr = void (*)(RTTIStackFrame&, RTTIFunction&);

/**
 * @brief Base interface for reflected callable entities in the RTTI system.
 *
 * This interface stores common function metadata and defines the abstract
 * invocation entry point used by stack-frame based dispatch.
 */
class JAYBIRD_API RTTIFunction {
 public:
  /**
   * @brief Constructs a reflected function descriptor with a name and optional flags.
   *
   * The name will be interned in the global name pool.
   *
   * @param name The name of the function.
   * @param flags The flags indicating properties of the function, such as whether it is native or a member function.
   */
  explicit RTTIFunction(std::string_view name, FunctionFlags flags = {}) noexcept;

  /**
   * @brief Destroys the reflected function descriptor.
   */
  virtual ~RTTIFunction() = default;

  /**
   * @brief Returns the reflected function name.
   *
   * @return This function returns the reflected function name.
   */
  [[nodiscard]] const Name& name() const noexcept;

  /**
   * @brief Returns the reflected function flags.
   *
   * @return This function returns the reflected function flags.
   */
  [[nodiscard]] FunctionFlags flags() const noexcept;

  /**
   * @brief Returns the reflected arguments in declaration order.
   *
   * @return This function returns raw pointers to arguments in declaration order.
   */
  [[nodiscard]] std::vector<const RTTIArgument*> arguments() const noexcept;

  /**
   * @brief Adds an argument to the function with the given name and type.
   *
   * The name will be interned in the global name pool.
   *
   * @param name The name of the argument to add.
   * @param type The reflected type descriptor for the argument.
   */
  void argument(std::string_view name, RTTIType* type) noexcept;

  /**
   * @brief Returns the reflected return type descriptor for this function.
   *
   * @return This function returns the reflected return type descriptor, or @code nullptr@endcode when the function has
   * no return value.
   */
  [[nodiscard]] const RTTIType* returnType() const noexcept;

  /**
   * @brief Sets the return type of the function.
   *
   * @param type The return type to set for the function as an @c RTTIType pointer.
   */
  void returnType(RTTIType* type) noexcept;

  /**
   * @brief Creates a new @c StackFrame suitable for invoking this function, with the appropriate number of argument
   * slots, return value slot (if applicable), and "this" pointer slot (for member functions).
   *
   * @return A new @c StackFrame initialized for invoking this function.
   */
  [[nodiscard]] RTTIStackFrame createStackFrame() const noexcept;

  /**
   * @brief Invokes the function using function-call syntax as a shorthand for @code invoke(frame)@endcode.
   *
   * @param frame The stack frame containing invocation context such as arguments, return storage, and optional
   * object pointer.
   */
  void operator()(RTTIStackFrame& frame);

  /**
   * @brief Invokes the function with the given stack frame, which contains the "this" pointer (for member functions),
   * argument values, and return value storage.
   *
   * @param frame The stack frame containing the necessary context for invocation, including the "this" pointer,
   * argument values, and return value storage.
   */
  virtual void invoke(RTTIStackFrame& frame) = 0;

 protected:
  /**
   * @brief This field stores the interned name of the function.
   */
  Name m_name;

  /**
   * @brief This field stores the flags that indicate properties of the function.
   */
  FunctionFlags m_flags;

  /**
   * @brief This collection stores shared pointers to @c RTTIArgument objects representing the function arguments.
   *
   * Each entry stores both the reflected argument name and its reflected type descriptor.
   */
  std::vector<std::shared_ptr<RTTIArgument>> m_arguments;

  /**
   * @brief This pointer refers to the return type of the function, or @c nullptr if the function has no return value.
   */
  RTTIType* m_return{};
};
}  // namespace core::rtti
