#pragma once

namespace core::rtti {

/**
 * @brief Classification of types in the RTTI system.
 */
enum class TypeKind : std::size_t {
  /**
   * @brief Interned string (IName) type.
   */
  NAME,

  /**
   * @brief Class or struct type with members.
   */
  CLASS,

  /**
   * @brief Dynamic array container type.
   */
  ARRAY,

  /**
   * @brief String sequence type.
   */
  STRING,

  /**
   * @brief Fundamental or primitive type (e.g., int, float, bool).
   */
  FUNDAMENTAL,
};

}  // namespace core::rtti
