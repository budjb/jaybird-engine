#pragma once
#include "IType.hpp"

namespace core::rtti {
class IContainerType : public IType {
 public:
  explicit IContainerType(const IString& name, std::size_t size, std::size_t alignment, const IType* inner) noexcept;

  ~IContainerType() override = default;

  [[nodiscard]] const IType* inner() const noexcept;

 private:
  const IType* m_inner;
};
}  // namespace core::rtti
