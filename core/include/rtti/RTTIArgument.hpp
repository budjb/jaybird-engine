#pragma once

#include "Name.hpp"
#include "RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Represents an argument to an RTTI function, consisting of a name and a type.
 */
class RTTIArgument {
 public:
  /**
   * @brief Constructs a @c RTTIArgument with the given name and type.
   *
   * @param name The interned name of the argument as a @code Name@endcode.
   * @param type The type of the argument as a pointer to an @code RTTIType@endcode.
   */
  RTTIArgument(const Name& name, const RTTIType* type) noexcept : m_name(name), m_type(type) {}

  /**
   * @brief Returns the interned name of the argument.
   *
   * @return The interned name of the argument as a @code Name@endcode.
   */
  [[nodiscard]] const Name& name() const noexcept {
    return m_name;
  }

  /**
   * @brief Returns the type of the argument.
   *
   * @return The type of the argument as a pointer to an @code RTTIType@endcode.
   */
  [[nodiscard]] const RTTIType* type() const noexcept {
    return m_type;
  }

 private:
  /**
   * @brief The interned name of the argument.
   */
  Name m_name;

  /**
   * @brief A pointer to the type of the argument.
   */
  const RTTIType* m_type;
};
}  // namespace core::rtti
