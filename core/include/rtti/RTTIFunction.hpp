#pragma once

#include <array>

#include "IName.hpp"
#include "INamePool.hpp"
#include "RTTIProperty.hpp"
#include "RTTIStackFrame.hpp"
#include "RTTITypeSystem.hpp"

namespace core::rtti {
/**
 * @brief Extracts traits from function types.
 *
 * @tparam T The function type to extract traits from.
 */
template <typename T>
struct FunctionTraits;

/**
 * @brief Extracts traits from function pointer types that are either global or static class members.
 *
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret (*)(Args...)> {
  /**
   * @brief The return type of the function.
   */
  using returnType = Ret;

  /**
   * @brief A tuple containing the argument types of the function.
   */
  using argumentTuple = std::tuple<Args...>;

  /**
   * @brief A template alias to extract the type of the argument at the given index.
   *
   * @tparam Index The index of the argument type to extract.
   */
  template <std::size_t Index>
  using argumentType = std::tuple_element_t<Index, argumentTuple>;

  /**
   * @brief Whether the function is a non-static member of a class.
   */
  static constexpr bool isMember = false;

  /**
   * @brief The number of arguments the function takes.
   */
  static constexpr std::size_t numArgs = sizeof...(Args);
};

/**
 * @brief Extracts traits from std::function types by treating them as function pointers.
 *
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret (*)(Args...)> {};

/**
 * @brief Extracts traits from member function pointer types, including the class type, return type, and argument types.
 *
 * @tparam ClassType The class type that the member function belongs to.
 * @tparam Ret The return type of the member function.
 * @tparam Args The argument types of the member function.
 */
template <typename ClassType, typename Ret, typename... Args>
struct FunctionTraits<Ret (ClassType::*)(Args...)> {
  /**
   * @brief The class type that the member function belongs to.
   */
  using classType = ClassType;

  /**
   * @brief The return type of the member function.
   */
  using returnType = Ret;

  /**
   * @brief A tuple containing the argument types of the member function.
   */
  using argumentTuple = std::tuple<Args...>;

  /**
   * @brief A template alias to extract the type of the argument at the given index.
   *
   * @tparam Index The index of the argument type to extract.
   */
  template <std::size_t Index>
  using argumentType = std::tuple_element_t<Index, argumentTuple>;

  /**
   * @brief Whether the function is a non-static member of a class.
   */
  static constexpr bool isMember = true;

  /**
   * @brief The number of arguments the member function takes.
   */
  static constexpr std::size_t numArgs = sizeof...(Args);
};

/**
 * @brief Extracts traits from const member function pointer types, including the class type, return type, and argument
 * types.
 *
 * @tparam ClassType The class type that the const member function belongs to.
 * @tparam Ret The return type of the const member function.
 * @tparam Args The argument types of the const member function.
 */
template <typename ClassType, typename Ret, typename... Args>
struct FunctionTraits<Ret (ClassType::*)(Args...) const> {
  /**
   * @brief The class type that the const member function belongs to.
   */
  using classType = const ClassType;

  /**
   * @brief The return type of the const member function.
   */
  using returnType = Ret;

  /**
   * @brief A tuple containing the argument types of the const member function.
   */
  using argumentTuple = std::tuple<Args...>;

  /**
   * @brief A template alias to extract the type of the argument at the given index.
   *
   * @tparam Index The index of the argument type to extract.
   */
  template <std::size_t Index>
  using argumentType = std::tuple_element_t<Index, argumentTuple>;

  /**
   * @brief Whether the function is a non-static member of a class.
   */
  static constexpr bool isMember = true;

  /**
   * @brief The number of arguments the const member function takes.
   */
  static constexpr std::size_t numArgs = sizeof...(Args);
};

/**
 * @brief Concept to check if a type is a member function pointer (non-static) based on the FunctionTraits
 * specialization.
 *
 * @tparam T The function pointer type to check.
 */
template <typename T>
concept MemberFunction = FunctionTraits<T>::isMember;

/**
 * @brief Concept that accepts non-member function pointer types.
 *
 * @tparam T This type is checked against @c FunctionTraits.
 */
template <typename T>
concept FreeFunction = !FunctionTraits<T>::isMember;

/**
 * @brief Flags to indicate properties of a function.
 */
struct FunctionFlags {
  /**
   * @brief Whether the function is a native C++ function that can be directly invoked, as opposed to a scripted or
   * interpreted function.
   */
  bool isNative : 1;

  /**
   * @brief Whether the function is a non-static member of a class, which requires an object instance to invoke and has
   * special handling for the "this" pointer in the stack frame.
   */
  bool isMember : 1;
};

/**
 * @brief Base interface for reflected callable entities in the RTTI system.
 *
 * This interface stores common function metadata and defines the abstract
 * invocation entry point used by stack-frame based dispatch.
 */
class RTTIFunction {
 public:
  /**
   * @brief Constructs a reflected function descriptor with a name and optional flags.
   *
   * The name will be interned in the global name pool.
   *
   * @param name The name of the function.
   * @param flags The flags indicating properties of the function, such as whether it is native or a member function.
   */
  explicit RTTIFunction(const std::string_view name, const FunctionFlags flags = {}) noexcept
      : m_name(INamePool::get().addName(name)), m_flags(flags) {
    m_flags.isMember = false;
  }

  /**
   * @brief Destroys the reflected function descriptor.
   */
  virtual ~RTTIFunction() = default;

  /**
   * @brief Returns the reflected function name.
   *
   * @return This function returns the reflected function name.
   */
  [[nodiscard]] const IName& name() const noexcept {
    return m_name;
  }

  /**
   * @brief Returns the reflected function flags.
   *
   * @return This function returns the reflected function flags.
   */
  [[nodiscard]] FunctionFlags flags() const noexcept {
    return m_flags;
  }

  /**
   * @brief Returns the reflected argument properties in declaration order.
   *
   * @return This function returns raw pointers to argument properties in declaration order.
   */
  [[nodiscard]] std::vector<const RTTIProperty*> arguments() const noexcept {
    std::vector<const RTTIProperty*> arguments;
    arguments.reserve(m_arguments.size());

    for (const auto& arg : m_arguments) {
      arguments.push_back(arg.get());
    }

    return arguments;
  }

  /**
   * @brief Adds an argument to the function with the given name and type.
   *
   * The name will be interned in the global name pool.
   *
   * @param name The name of the argument to add.
   * @param type The reflected type descriptor for the argument.
   */
  void argument(const std::string_view name, RTTIType* type) noexcept {
    m_arguments.push_back(std::make_unique<RTTIProperty>(INamePool::get().addName(name), type));
  }

  /**
   * @brief Returns the reflected return type descriptor for this function.
   *
   * @return This function returns the reflected return type descriptor, or @code nullptr@endcode when the function has
   * no return value.
   */
  [[nodiscard]] const RTTIType* returnType() const noexcept {
    return m_return;
  }

  /**
   * @brief Sets the return type of the function.
   *
   * @param type The return type to set for the function as an @c RTTIType pointer.
   */
  void returnType(RTTIType* type) noexcept {
    m_return = type;
  }

  /**
   * @brief Creates a new @c StackFrame suitable for invoking this function, with the appropriate number of argument
   * slots, return value slot (if applicable), and "this" pointer slot (for member functions).
   *
   * @return A new @c StackFrame initialized for invoking this function.
   */
  [[nodiscard]] RTTIStackFrame createStackFrame() const noexcept {
    return RTTIStackFrame(m_arguments.size(), m_return != nullptr, m_flags.isMember);
  }

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
   * @brief The interned name of the function.
   */
  IName m_name;

  /**
   * @brief Flags indicating properties of the function.
   */
  FunctionFlags m_flags;

  /**
   * @brief A vector of unique pointers to @c Property objects representing the arguments of the function, including
   * their names and types.
   */
  std::vector<std::unique_ptr<RTTIProperty>> m_arguments;

  /**
   * @brief A pointer to the return type of the function, or @c nullptr if the function has no return type.
   */
  RTTIType* m_return{};
};

/**
 * @brief An implementation of @c RTTIClassFunction that wraps a native C++ member function pointer, allowing it to be
 * invoked through the RTTI system.
 *
 * This class automatically handles the introspection of the function signature to register argument types and the
 * return type, and implements the invocation logic to call the native member function using the provided stack frame.
 *
 * Due to C++ limitations, the names of arguments cannot be deduced from the function pointer type, so they must be
 * provided explicitly when constructing a @c NativeClassFunction. A name for each argument of the function must be
 * provided when constructing this class.
 *
 * @tparam F The type of the member function pointer to wrap, which must satisfy the @c MemberFunction concept.
 * @tparam TBase The reflected function base descriptor type.
 */
template <typename F, typename TBase>
class RTTITFunction : public TBase {
 public:
  /**
   * @brief Traits extracted from the member function pointer type @c F, including the class type, return type, argument
   * types, and other properties.
   */
  using traits = FunctionTraits<F>;

  /**
   * @brief Constructs a @c NativeClassFunction by wrapping the given member function pointer and registering its
   * argument types and return type.
   *
   * Due to C++ limitations, the names of arguments cannot be deduced from the function pointer type, so they must be
   * provided explicitly when constructing a @c NativeClassFunction. A name for each argument of the function must be
   * provided when constructing this class.
   *
   * @tparam ArgNames The types of the argument names, which must be convertible to @c std::string_view and match the
   * number of arguments in the function signature.
   * @param name The name of the function.
   * @param function The native member function pointer to wrap, which must satisfy the @c MemberFunction concept.
   * @param argNames The names of the arguments corresponding to the function's parameters, which must be provided in
   * the same order as the parameters in the function signature.
   * @throws std::runtime_error If any argument type or the return type (if applicable) cannot be found in the type
   * registry, or if the number of provided argument names does not match the number of arguments in the function
   * signature.
   */
  template <typename... ArgNames>
    requires(sizeof...(ArgNames) == traits::numArgs && (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit RTTITFunction(const std::string_view name, F function, ArgNames&&... argNames)
      : TBase(name), m_function(function) {
    this->m_flags.isNative = true;

    if constexpr (!std::is_void_v<typename traits::returnType>) {
      if (this->m_return = TypeResolver<typename traits::returnType>::get(); !this->m_return) {
        throw std::runtime_error("Return type not found in type registry for function: " + std::string(name));
      }
    }

    const std::array<std::string_view, traits::numArgs> argumentNames{
        std::string_view(std::forward<ArgNames>(argNames))...};

    initializeArguments(name, argumentNames, std::make_index_sequence<traits::numArgs>{});
  }

  /**
   * @brief Invokes the native member function using the provided stack frame, which contains the "this" pointer,
   * argument values, and return value storage.
   *
   * @param frame The stack frame containing the necessary context for invocation, including the "this" pointer,
   * argument values, and return value storage.
   * @throws std::runtime_error If the "this" pointer is invalid for member function invocation, or if the return
   * pointer is invalid for member functions with a non-void return type.
   */
  void invoke(RTTIStackFrame& frame) override {
    invoke(frame, std::make_index_sequence<traits::numArgs>{});
  }

 private:
  /**
   * @brief Registers every reflected argument type for this native member function.
   *
   * This helper expands the compile-time argument index pack and forwards each
   * index to @c initializeArgument.
   *
   * @tparam Indices The compile-time indices of the arguments to register.
   * @param functionName The name of the function.
   * @param argumentNames The array of argument names corresponding to the function's parameters.
   */
  template <std::size_t... Indices>
  void initializeArguments(const std::string_view functionName,
                           const std::array<std::string_view, traits::numArgs>& argumentNames,
                           std::index_sequence<Indices...>) {
    (initializeArgument<Indices>(functionName, argumentNames[Indices]), ...);
  }

  /**
   * @brief Registers one reflected argument type for this native member function.
   *
   * This helper resolves the argument name and type descriptor, then stores a
   * matching @c Property in @c m_arguments.
   *
   * @tparam Index The compile-time index of the argument to register.
   * @param functionName The name of the function.
   * @param argumentName The name of the argument.
   * @throws std::runtime_error If the argument type cannot be found in the type registry.
   */
  template <std::size_t Index>
  void initializeArgument(const std::string_view functionName, const std::string_view argumentName) {
    using ArgType = traits::template argumentType<Index>;

    auto* argType = TypeResolver<ArgType>::get();

    if (!argType) {
      throw std::runtime_error("Argument type not found in type registry for argument " + std::to_string(Index) +
                               " in function: " + std::string(functionName));
    }

    TBase::argument(argumentName, argType);
  }

  /**
   * @brief Invokes the native member function using the provided stack frame and compile-time argument indices, which
   * contains the "this" pointer, argument values, and return value storage.
   *
   * This helper is used to expand the argument index pack and forward the arguments from the stack frame to
   * the member function invocation. It handles both void and non-void return types appropriately.
   *
   * @tparam Indices The compile-time indices of the arguments to pass to the member function.
   * @param frame The stack frame containing the necessary context for invocation, including the "this" pointer,
   * argument values, and return value storage.
   * @throws std::runtime_error If the "this" pointer is invalid for member function invocation, or if the return
   * pointer is invalid for member functions with a non-void return type.
   */
  template <std::size_t... Indices>
  void invoke(RTTIStackFrame& frame, std::index_sequence<Indices...>) {
    if constexpr (traits::isMember) {
      typename traits::classType* obj = frame.thisPtr<typename traits::classType>();

      if (!obj) {
        throw std::runtime_error("Invalid object pointer for member function invocation.");
      }

      if constexpr (std::is_void_v<typename traits::returnType>) {
        std::invoke(m_function, obj, *getArg<typename traits::template argumentType<Indices>>(frame, Indices)...);
      } else {
        auto* returnPtr = frame.returnPtr<typename traits::returnType>();

        if (!returnPtr) {
          throw std::runtime_error("Invalid return pointer for member function invocation.");
        }

        *returnPtr =
            std::invoke(m_function, obj, *getArg<typename traits::template argumentType<Indices>>(frame, Indices)...);
      }
    } else {
      if constexpr (std::is_void_v<typename traits::returnType>) {
        std::invoke(m_function, *getArg<typename traits::template argumentType<Indices>>(frame, Indices)...);
      } else {
        auto* returnPtr = frame.returnPtr<typename traits::returnType>();

        if (!returnPtr) {
          throw std::runtime_error("Invalid return pointer for function invocation.");
        }

        *returnPtr =
            std::invoke(m_function, *getArg<typename traits::template argumentType<Indices>>(frame, Indices)...);
      }
    }
  }

  /**
   * @brief Helper to retrieve an argument pointer from the stack frame for the given index and type, with error
   * checking.
   *
   * @tparam T The expected type of the argument pointer.
   * @param frame The stack frame containing the argument pointers.
   * @param index The index of the argument to retrieve.
   * @return A pointer to the argument at the specified index in the stack frame, of the expected type.
   * @throws std::runtime_error If the argument pointer at the specified index is missing (i.e., nullptr) in the stack
   * frame.
   */
  template <typename T>
  T* getArg(const RTTIStackFrame& frame, const std::size_t index) {
    auto* value = frame.argPtr<T>(index);

    if (!value) {
      throw std::runtime_error("Missing argument pointer at index " + std::to_string(index) +
                               " for member function invocation.");
    }

    return value;
  }

  /**
   * @brief Bring parent @c m_flags into scope.
   */
  using TBase::m_flags;

  /**
   * @brief Bring parent @c m_arguments into scope.
   */
  using TBase::m_arguments;

  /**
   * @brief Bring parent @c m_return into scope.
   */
  using TBase::m_return;

  /**
   * @brief The native member function pointer wrapped by this class, which is invoked when the @c invoke method is
   * called.
   */
  F m_function;
};
}  // namespace core::rtti
