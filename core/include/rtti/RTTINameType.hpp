#pragma once

#include "NamePool.hpp"
#include "rtti/RTTIName.hpp"
#include "rtti/RTTIType.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief Concrete RTTI descriptor for the @code Name@endcode value type.
 *
 * This descriptor models @code Name@endcode as a named value-type descriptor with kind
 * @code RTTITypeKind::NAME@endcode.
 */
class RTTINameType : public TypedRTTIType<Name, RTTIType> {
 public:
  /**
   * @brief Constructs an @code RTTNameType@endcode descriptor with the canonical @code Name@endcode type name.
   */
  explicit RTTINameType() noexcept : TypedRTTIType(NamePool::get().addName(GetRTTIName<Name>()), RTTITypeKind::NAME) {}

  /**
   * @brief Destroys the @code RTTNameType@endcode descriptor.
   */
  ~RTTINameType() override = default;
};
}  // namespace core::rtti
