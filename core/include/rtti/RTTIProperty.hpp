#pragma once

#include "rtti/RTTIType.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief Represents a property of an RTTI class, consisting of a name and a type.
 */
class JAYBIRD_API RTTIProperty {
 public:
  /**
   * @brief Constructs a @c RTTIProperty with the given name and type.
   *
   * @param name The interned name of the property as a @code Name@endcode.
   * @param type The type of the property as a pointer to an @code RTTIType@endcode.
   */
  RTTIProperty(const Name& name, const RTTIType* type) noexcept : m_name(name), m_type(type) {}

  /**
   * @brief Returns the interned name of the property.
   *
   * @return The interned name of the property as a @code Name@endcode.
   */
  [[nodiscard]] const Name& name() const noexcept {
    return m_name;
  }

  /**
   * @brief Returns the type of the property.
   *
   * @return The type of the property as a pointer to an @code RTTIType@endcode.
   */
  [[nodiscard]] const RTTIType* type() const noexcept {
    return m_type;
  }

 private:
  /**
   * @brief This field stores the interned name of the property, which serves as its unique identifier within the RTTI
   * system.
   */
  Name m_name;

  /**
   * @brief This pointer refers to the type of the property, enabling type introspection and dynamic type checking.
   */
  const RTTIType* m_type;
};
}  // namespace core::rtti
