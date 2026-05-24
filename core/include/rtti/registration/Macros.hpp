#pragma once

#include "AutoRegistration.hpp"
#include "FixedString.hpp"
#include "TypeDefinition.hpp"
#include "TypeName.hpp"

/**
 * @brief Macro for registering a type name mapping.
 *
 * @param _type The type for which to create the name mapping.
 * @param _name The name to associate with the type in the RTTI system.
 */
#define RTTI_REGISTER_NAME(_type, _name)             \
  template <>                                        \
  struct core::rtti::TypeName<_type> {               \
    static constexpr core::FixedString value{_name}; \
  }

/**
 * @brief A simple helper macro that concatenates tokens together.
 *
 * @param a The left hand side of the concatenation.
 * @param b The right hand side of the concatenation.
 * @return The result of concatenating the two tokens together.
 */
#define X_CAT(lhs, rhs) lhs##rhs

/**
 * @brief A helper macro that counts the number of arguments passed to it, up to a maximum of 3. This is used to
 * determine which overload of a macro to invoke based on the number of arguments provided.
 *
 * As an example, calling @code X_ARG_COUNT(A, B)@endcode would expand to @c 2, while calling @code X_ARG_COUNT(A, B,
 * C)@endcode would expand to @code 3@endcode.
 */
#define X_ARG_COUNT_IMPL(_1, _2, _3, N, ...) N

/**
 * @brief A helper macro that counts the number of arguments passed to it, up to a maximum of 3. This macro is used in
 * conjunction with the X_CAT macro to select the appropriate overload of a macro based on the number of arguments
 * provided. The macro works by appending the numbers 3, 2, and 1 to the list of arguments, and then using the
 * X_ARG_COUNT_IMPL macro to extract the correct count based on the position of the arguments.
 *
 * For example, if 2 arguments are provided, the macro will expand to @c 2, and if 3 arguments are provided, it will
 * expand to @c 3. This allows for flexible macro definitions that can handle different numbers of arguments without
 * requiring the user to specify the count explicitly.
 */
#define X_ARG_COUNT(...) X_ARG_COUNT_IMPL(__VA_ARGS__, 3, 2, 1)

/**
 * @brief Macro for registering a class type with the RTTI system. This macro supports multiple overloads to allow for
 * different levels of detail in the registration process.
 *
 * At least 2 arguments are required: the first argument should be the class type being registered, and the second is a
 * block that defines the class. This requires that the class contain a public, static property @c NAME in the shape of
 * a constexpr char array. This form takes the signature:
 * @code
 * RTTI_REGISTER_CLASS(SomeClass, {
 *   // ...
 * });@endcode
 *
 * An optional third argument can be provided to explicitly specify a custom name for the class in the RTTI system. This
 * signature is:
 * @code
 * RTTI_REGISTER_CLASS(SomeClass, "ClassName", {
 *   // ...
 * });@endcode
 */
#define RTTI_REGISTER_CLASS(...) X_CAT(X_RTTI_REGISTER_CLASS_, X_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

/**
 * @brief Macro for registering a class type with the RTTI system using the class's static @c NAME property as the name
 * in the RTTI system.
 *
 * @param _type The class type to register with the RTTI system. This class must contain a public, static property @c
 * NAME that is a constexpr char array, which will be used as the name of the class in the RTTI system.
 * @param _desc A block of code that defines the class type in the RTTI system, such as its properties, functions,
 * parent relationships, etc.
 */
#define X_RTTI_REGISTER_CLASS_2(_type, _desc) X_RTTI_REGISTER_CLASS_3(_type, _type::NAME, _desc)

/**
 * @brief Macro for registering a class type with the RTTI system with the provided name.
 *
 * @param _type The class type to register with the RTTI system.
 * @param _name The name to associate with the class type in the RTTI system.
 * @param _desc A block of code that defines the class type in the RTTI system, such as its properties, functions,
 * parent relationships, etc.
 */
#define X_RTTI_REGISTER_CLASS_3(_type, _name, _desc)                          \
  RTTI_REGISTER_TYPE(_type, _name);                                           \
                                                                              \
  template <>                                                                 \
  class core::rtti::AutoRegistration<core::rtti::IClassDefinition<_type>{}> { \
   public:                                                                    \
    static void define(core::rtti::IClassType* type) {                        \
      using T = _type;                                                        \
      _desc                                                                   \
    };                                                                        \
  }

/**
 * @brief Macro for registering a fundamental type with the RTTI system. This macro creates a name mapping for the type
 * and also creates an AutoRegistration specialization for the type, which will trigger the registration process when
 * the type is loaded.
 *
 * @param _type The fundamental type to register with the RTTI system. This should be a built-in type such as int,
 * float, etc.
 * @param _name The name to associate with the fundamental type in the RTTI system.
 */
#define RTTI_REGISTER_TYPE(_type, _name) \
  RTTI_REGISTER_NAME(_type, _name);      \
                                         \
  template <>                            \
  class core::rtti::AutoRegistration<core::rtti::IFundamentalDefinition<_type>{}> {};
