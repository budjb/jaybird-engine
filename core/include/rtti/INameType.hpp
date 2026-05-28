#pragma once

#include "IName.hpp"
#include "INamePool.hpp"
#include "rtti/IType.hpp"
#include "rtti/TypeName.hpp"

namespace core::rtti {
/**
 * @brief Concrete RTTI descriptor for the @code IName@endcode value type.
 *
 * This descriptor models @code IName@endcode as a named value-type descriptor with kind @code TypeKind::NAME@endcode.
 */
class INameType : public TType<IName, IType> {
 public:
  /**
   * @brief Constructs an @code INameType@endcode descriptor with the canonical @code IName@endcode type name.
   */
  explicit INameType() noexcept : TType(INamePool::get().addName(GetTypeName<IName>()), TypeKind::NAME) {}

  /**
   * @brief Destroys the @code INameType@endcode descriptor.
   */
  ~INameType() override = default;
};
}  // namespace core::rtti
