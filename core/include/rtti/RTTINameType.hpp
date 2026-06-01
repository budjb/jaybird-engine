#pragma once

#include "Name.hpp"
#include "NamePool.hpp"
#include "rtti/RTTITType.hpp"
#include "rtti/RTTIType.hpp"
#include "rtti/RTTITypeName.hpp"

namespace core::rtti {
/**
 * @brief Concrete RTTI descriptor for the @code Name@endcode value type.
 *
 * This descriptor models @code Name@endcode as a named value-type descriptor with kind
 * @code RTTITypeKind::NAME@endcode.
 */
class RTTINameType : public RTTITType<Name, RTTIType> {
 public:
  /**
   * @brief Constructs an @code RTTNameType@endcode descriptor with the canonical @code Name@endcode type name.
   */
  explicit RTTINameType() noexcept : RTTITType(NamePool::get().addName(GetTypeName<Name>()), RTTITypeKind::NAME) {}

  /**
   * @brief Destroys the @code RTTNameType@endcode descriptor.
   */
  ~RTTINameType() override = default;
};
}  // namespace core::rtti
