#pragma once

#include "RawVector.hpp"
#include "rtti/RTTI.hpp"

namespace core::container {
class RTTIVector : public RawVector {
 public:
  explicit RTTIVector(const rtti::IArray* arrayType,
                      const std::pmr::polymorphic_allocator<> allocator = std::pmr::get_default_resource()) noexcept
      : RawVector(arrayType->inner()->size(), arrayType->inner()->alignment(), allocator),
        m_arrayType(arrayType),
        m_innerType(arrayType->inner()) {}

  ~RTTIVector() override = default;

  [[nodiscard]] const rtti::IArray* arrayType() const noexcept {
    return m_arrayType;
  }

  [[nodiscard]] const rtti::IType* innerType() const noexcept {
    return m_innerType;
  }

  [[nodiscard]] std::size_t maxSize() const noexcept override {
    if (m_elementSize == 0) {
      return 0;
    }

    return traits::max_size(m_allocator) / m_elementSize;
  }

 protected:
  void constructAt(void* destination) noexcept override {
    m_innerType->construct(destination);
  }

  void constructAt(void* destination, void* source) noexcept override {
    // TODO: create copy constructor function
    m_innerType->construct(destination);
  }
  void copyConstructAt(void* destination, const void* source) override {
    // TODO: create copy constructor
    m_innerType->construct(destination);
  }

  void destroyAt(void* element) noexcept override {
    m_innerType->destroy(element);
  }

 private:
  const rtti::IArray* m_arrayType;
  const rtti::IType* m_innerType;
};
}  // namespace core::container
