#pragma once

#include "IString.hpp"
#include "TypeKind.hpp"

namespace core::rtti {
class IArrayType;

/**
 * @brief Interface representing a type in the RTTI system. This class provides information about the type, such as its
 * name, size, alignment, and kind. It also defines virtual functions for assigning values, constructing and destructing
 * instances of the type.
 */
class IType {
 public:
  explicit IType(const IString& name, std::size_t size, TypeKind kind) noexcept;

  explicit IType(const IString& name, std::size_t size, std::size_t alignment, TypeKind kind) noexcept;

  virtual ~IType() = default;

  [[nodiscard]] std::size_t size() const noexcept;

  [[nodiscard]] std::size_t alignment() const noexcept;

  [[nodiscard]] TypeKind kind() const noexcept;

  [[nodiscard]] IString name() const noexcept;

  virtual void assign(void* dst, const void* src) = 0;

  virtual void* create() = 0;

  virtual void free(void* memory) = 0;

  virtual void destroy(void* memory) noexcept = 0;

  virtual void construct(void* memory) noexcept = 0;

  virtual bool equals(const void* lhs, const void* rhs) const noexcept = 0;

  // [[nodiscard]] IArray* asArray() const noexcept;

  bool operator==(const IType& type) const noexcept;

  bool operator!=(const IType& type) const noexcept;

 private:
  const IString m_name;
  const std::size_t m_size;
  const std::size_t m_alignment;
  const TypeKind m_kind;
};
}  // namespace core::rtti
