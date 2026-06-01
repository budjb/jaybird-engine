#pragma once

#include "Export.hpp"
#include "NamePool.hpp"
#include "RTTITType.hpp"
#include "RTTIType.hpp"
#include "RTTITypeName.hpp"

namespace core::rtti {
/**
 * @brief Polymorphic interface for class type descriptors in the RTTI system.
 *
 * Concrete class-type descriptors derive from this interface, while shared implementation lives in @code
 * TTypeImpl@endcode.
 */
class JAYBIRD_API RTTIClassType : public RTTIType {
 public:
  /**
   * @brief Constructs an @c RTTIClassType with the given metadata.
   *
   * @param name The name of the class type, represented as an Name.
   * @param size The size of the class type in bytes.
   * @param alignment The alignment requirement of the class type in bytes.
   */
  explicit RTTIClassType(const Name& name, std::size_t size, std::size_t alignment) noexcept;

  /**
   * @brief Virtual destructor for the @c RTTIClassType interface.
   */
  ~RTTIClassType() override = default;
};

/**
 * @brief A template class representing a class/struct type in the RTTI system.
 *
 * @tparam T The underlying class type that this @c TClassType represents.
 */
template <typename T>
  requires std::is_class_v<T>
class RTTIClassTType : public RTTITType<T, RTTIClassType> {
 public:
  /**
   * @brief Defines a type alias for the underlying type @code T@endcode.
   */
  using Type = T;

  /**
   * @brief Constructs a @c TClassType for the specified type @code T@endcode.
   */
  explicit RTTIClassTType() : RTTITType<T, RTTIClassType>(NamePool::get().addName(GetTypeName<T>())) {}
};
}  // namespace core::rtti
