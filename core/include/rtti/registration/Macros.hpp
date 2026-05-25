#pragma once

#include "AutoRegistration.hpp"
#include "CString.hpp"
#include "TypeDefinition.hpp"
#include "TypeName.hpp"

/**
 * @brief Select the Nth argument from the provided arguments. This is used for macro overloading based on argument
 * count.
 */
#define X_SELECT(_0, _1, _2, _3, ...) _3

/**
 * @brief Recompose the arguments for selection. This is used in conjunction with X_SELECT to enable macro overloading
 * based on argument count.
 */
#define X_RECOMPOSE(_) X_SELECT _

/**
 * @brief Expand the arguments for the case when no arguments are provided. This is used to ensure that the correct
 * macro variant is selected when no arguments are given, by providing dummy arguments that will lead to the selection
 * of the correct macro variant for zero arguments.
 */
#define X_EXPAND_NOARG(NAME) , , , NAME##_1

/**
 * @brief Select the correct macro variant based on the number of arguments provided. This macro takes the base name of
 * the macro and the provided arguments, and it uses the number of arguments to select the appropriate macro variant
 * (e.g., NAME_1, NAME_2, NAME_3, etc.).
 */
#define X_SELECT_BY_NARG(NAME, ...) X_RECOMPOSE((__VA_ARGS__, NAME##_3, NAME##_2, NAME##_1, ))

/**
 * @brief Select the correct macro variant based on the number of arguments provided, and invoke it with the provided
 * arguments.
 */
#define X_SELECT_MACRO(NAME, ...) X_SELECT_BY_NARG(NAME, X_EXPAND_NOARG __VA_ARGS__(NAME))

/**
 * @brief Macro for overloading based on the number of arguments. This macro takes the base name of the macro and the
 * provided arguments, and it uses the number of arguments to select the appropriate macro variant and invoke it with
 * the provided arguments.
 */
#define X_OVERLOAD(NAME, ...) X_SELECT_MACRO(NAME, __VA_ARGS__)(__VA_ARGS__)

/**
 * @brief Macro for registering a type name mapping.
 *
 * @param _type The type for which to create the name mapping.
 * @param _name The name to associate with the type in the RTTI system. Can be a const char*,
 *              a string literal (const char[N]), or a CString<N>.
 */
#define RTTI_REGISTER_NAME(_type, _name) \
  template <>                            \
  struct core::rtti::TypeName<_type> {   \
    static constexpr auto value{_name};  \
  }

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
#define RTTI_REGISTER_CLASS(...) X_OVERLOAD(X_RTTI_REGISTER_CLASS, __VA_ARGS__)

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
  RTTI_REGISTER_NAME(_type, _name);                                           \
                                                                              \
  template <>                                                                 \
  class core::rtti::AutoRegistration<core::rtti::IClassDefinition<_type>{}> { \
   public:                                                                    \
    using Type = _type;                                                       \
    static void define(core::rtti::IClassType* type) {                        \
      using T = _type;                                                        \
      _desc                                                                   \
    };                                                                        \
  };

/**
 * @brief Macro for registering a fundamental type with the RTTI system. This macro creates a name mapping for the type
 * and also creates an AutoRegistration specialization for the type, which will trigger the registration process when
 * the type is loaded.
 *
 * @param _type The fundamental type to register with the RTTI system. This should be a built-in type such as int,
 * float, etc.
 * @param _name The name to associate with the fundamental type in the RTTI system.
 */
#define RTTI_REGISTER_TYPE(_type, _name)                                            \
  RTTI_REGISTER_NAME(_type, _name);                                                 \
                                                                                    \
  template <>                                                                       \
  class core::rtti::AutoRegistration<core::rtti::IFundamentalDefinition<_type>{}> { \
   public:                                                                          \
    using Type = _type;                                                             \
  };
