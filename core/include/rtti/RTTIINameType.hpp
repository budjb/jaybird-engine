#pragma once

#include "IName.hpp"
#include "INamePool.hpp"
#include "rtti/RTTITType.hpp"
#include "rtti/RTTIType.hpp"
#include "rtti/RTTITypeName.hpp"

namespace core::rtti {
/**
 * @brief Concrete RTTI descriptor for the @code IName@endcode value type.
 *
 * This descriptor models @code IName@endcode as a named value-type descriptor with kind
 * @code RTTITypeKind::NAME@endcode.
 */
class RTTIINameType : public RTTITType<IName, RTTIType> {
 public:
  /**
   * @brief Constructs an @code RTTIINameType@endcode descriptor with the canonical @code IName@endcode type name.
   */
  explicit RTTIINameType() noexcept : RTTITType(INamePool::get().addName(GetTypeName<IName>()), RTTITypeKind::NAME) {}

  /**
   * @brief Destroys the @code RTTIINameType@endcode descriptor.
   */
  ~RTTIINameType() override = default;
};
}  // namespace core::rtti
