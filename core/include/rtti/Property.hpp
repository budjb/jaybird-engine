#pragma once
#include "IName.hpp"
#include "RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Represents a named type, which is useful for class properties and function arguments.
 */
class Property {
 public:
  /**
   * @brief Constructs a @c Property with the given name and type.
   *
   * @param name The interned name of the property as an @c IName.
   * @param type The type of the property as a pointer to an @c RTTIType.
   */
  Property(const IName& name, const RTTIType* type) noexcept : m_name(name), m_type(type) {}

  /**
   * @brief Returns the interned name of the property.
   *
   * @return The interned name of the property as an @c IName.
   */
  [[nodiscard]] const IName& name() const noexcept {
    return m_name;
  }

  /**
   * @brief Returns the type of the property.
   *
   * @return The type of the property as a pointer to an @c RTTIType.
   */
  [[nodiscard]] const RTTIType* type() const noexcept {
    return m_type;
  }

 private:
  /**
   * @brief The interned name of the property, which serves as a unique identifier for the property within the RTTI
   * system.
   */
  IName m_name;

  /**
   * @brief A pointer to the type of the property, which describes the data type of the property and allows for type
   * introspection and dynamic type checking within the RTTI system.
   */
  const RTTIType* m_type;
};
}  // namespace core::rtti
