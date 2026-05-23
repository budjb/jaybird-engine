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
   * @brief Represents a string type, which is a sequence of characters. This is distinct from @c ISTRING, which is an
   * interned string used for identifiers and other purposes.
   */
  STRING,

  /**
   * @brief Represents a simple type, such as an integer, float, or other primitive type that does not have member
   * variables or functions. This kind of type is typically used for basic data types that can be directly assigned and
   * do not require complex construction or destruction logic.
   */
  SIMPLE,
};

}  // namespace core::rtti
