#pragma once

#include "IType.hpp"
#include "Export.hpp"
#include "TType.hpp"

namespace core::rtti {
/**
 * @brief Polymorphic interface for class type descriptors in the RTTI system.
 *
 * Concrete class-type descriptors derive from this interface, while shared implementation lives in @c TTypeImpl .
 */
class JAYBIRD_API IClassType : public IType {
 public:
  /**
   * @brief Constructs an @c IClassType with the given metadata.
   *
   * @param name The name of the class type, represented as an IName.
   * @param size The size of the class type in bytes.
   * @param alignment The alignment requirement of the class type in bytes.
   */
  explicit IClassType(const IName& name, const std::size_t size, const std::size_t alignment) noexcept
      : IType(name, size, alignment, TypeKind::CLASS) {}

  /**
   * @brief Virtual destructor for the @c IClassType interface.
   */
  ~IClassType() override = default;
};

/**
 * @brief A template class representing a class/struct type in the RTTI system.
 *
 * @tparam T The underlying class type that this @c TClassType represents.
 */
template <typename T>
  requires std::is_class_v<T>
class TClassType : public TType<T, IClassType> {
 public:
  /**
   * @brief Defines a type alias for the underlying type T.
   */
  using Type = T;

  /**
   * @brief Constructs a TClassType for the specified type @c T.
   *
   * @param name The name of the type, represented as an IName.
   */
  explicit TClassType(const IName& name) : TType<T, IClassType>(name) {}
};
}  // namespace core::rtti
