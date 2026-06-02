#pragma once

#include <array>
#include <functional>

#include "rtti/RTTIClassFunction.hpp"
#include "rtti/RTTIFunction.hpp"
#include "rtti/RTTIGlobalFunction.hpp"
#include "rtti/RTTITypeResolver.hpp"

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
  static constexpr bool isStatic = true;

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
   * @brief Whether the function is a static member of a class.
   */
  static constexpr bool isStatic = false;

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
  static constexpr bool isStatic = false;

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
concept MemberFunction = !FunctionTraits<T>::isStatic;

/**
 * @brief Concept that accepts non-member function pointer types.
 *
 * @tparam T This type is checked against @c FunctionTraits.
 */
template <typename T>
concept StaticFunction = FunctionTraits<T>::isStatic;

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
class TypedRTTIFunction : public TBase {
 public:
  /**
   * @brief Brings the base call-operator shortcut into this concrete wrapper's public interface.
   */
  using TBase::operator();

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
    requires(sizeof...(ArgNames) == FunctionTraits<F>::numArgs &&
             (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit TypedRTTIFunction(std::string_view name, F function, ArgNames&&... argNames)
      : TBase(name), m_function(function) {
    this->m_flags.isNative = true;
    this->m_flags.isStatic = traits::isStatic;

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
   * matching @c RTTIArgument in @code m_arguments@endcode.
   *
   * @tparam Index The compile-time index of the argument to register.
   * @param functionName The name of the function.
   * @param argumentName The name of the argument.
   * @throws std::runtime_error If the argument type cannot be found in the type registry.
   */
  template <std::size_t Index>
  void initializeArgument(std::string_view functionName, std::string_view argumentName) {
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
    if constexpr (traits::isStatic) {
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
    } else {
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

/**
 * @brief A concrete implementation of @c RTTIClassFunction that wraps a native C++ member function pointer, allowing it
 * to be invoked through the RTTI system.
 *
 * @tparam F The type of the member function pointer to wrap.
 */
template <typename F>
class TypedRTTIClassFunction : public TypedRTTIFunction<F, RTTIClassFunction> {
 public:
  /**
   * @brief Traits extracted from the member function pointer type @c F, including the class type, return type, argument
   * types, and other properties.
   */
  using traits = FunctionTraits<F>;

  /**
   * @brief Constructs a @c TypedRTTIClassFunction by wrapping the given member function pointer and registering its
   * argument types and return type.
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
  explicit TypedRTTIClassFunction(const std::string_view name, F function, ArgNames&&... argNames)
      : TypedRTTIFunction<F, RTTIClassFunction>(name, function, std::forward<ArgNames>(argNames)...) {}
};

/**
 * @brief Concrete reflected wrapper for free functions.
 *
 * @tparam F The free-function pointer type to wrap.
 */
template <StaticFunction F>
class TypedRTTIGlobalFunction : public TypedRTTIFunction<F, RTTIGlobalFunction> {
 public:
  /**
   * @brief Constructs a reflected wrapper for a global function from a function name.
   *
   * @tparam ArgNames These types provide one name per function argument.
   * @param name This value is the reflected function name.
   * @param function This value is the global function pointer to wrap.
   * @param argNames These values provide argument names in declaration order.
   */
  template <typename... ArgNames>
    requires(StaticFunction<F> && sizeof...(ArgNames) == FunctionTraits<F>::numArgs &&
             (std::convertible_to<ArgNames, std::string_view> && ...))
  explicit TypedRTTIGlobalFunction(std::string_view name, F function, ArgNames&&... argNames)
      : TypedRTTIFunction<F, RTTIGlobalFunction>(name, function, std::forward<ArgNames>(argNames)...) {}
};
}  // namespace core::rtti
