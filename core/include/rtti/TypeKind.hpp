#pragma once

namespace core::rtti {

/**
 * @brief Enumeration representing the kind of type in the RTTI system.
 */
enum class TypeKind : std::size_t {
  /**
   * @brief Represents an interned string type.
   */
  NAME,

  /**
   * @brief Represents a class type with member variables and functions.
   */
  CLASS,

  /**
   * @brief Represents an array type, which can hold multiple elements of a specified type.
   */
  ARRAY,

  /**
   * @brief Represents a string type, which is a sequence of characters.
   */
  STRING,

  /**
   * @brief Represents a fundamental type, such as an integer, float, or other primitive type that does not have member
   * variables or functions.
   */
  FUNDAMENTAL,
};

}  // namespace core::rtti
