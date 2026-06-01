#pragma once

#include "NamePool.hpp"
#include "rtti/RTTIName.hpp"
#include "rtti/RTTIType.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief Concrete RTTI descriptor for the @code std::string@endcode value type.
 *
 * This descriptor models @code std::string@endcode as a named value-type descriptor with kind
 * @code RTTITypeKind::STRING@endcode.
 */
class RTTIStringType : public TypedRTTIType<std::string, RTTIType> {
 public:
  /**
   * @brief Constructs an @code RTTStringType@endcode descriptor with the canonical @code std::string@endcode type name.
   */
  explicit RTTIStringType() noexcept
      : TypedRTTIType(NamePool::get().addName(GetRTTIName<std::string>()), RTTITypeKind::STRING) {}

  /**
   * @brief Destroys the @code RTTStringType@endcode descriptor.
   */
  ~RTTIStringType() override = default;
};
}  // namespace core::rtti
