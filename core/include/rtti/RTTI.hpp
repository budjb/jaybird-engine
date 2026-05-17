#pragma once

#include "IString.hpp"

namespace core::rtti {
/**
 * @brief Enumeration representing the kind of type in the RTTI system.
 */
enum class TypeKind : std::size_t {
  /**
   * @brief Represents an interned string type.
   */
  ISTRING,

  /**
   * @brief Represents a class type with member variables and functions.
   */
  CLASS,

  /**
   * @brief Represents an array type, which can hold multiple elements of a specified type.
   */
  ARRAY,

  /**
   * @brief Represents a string type, which is a sequence of characters. This is distinct from @c ISTRING, which is an
   * interned string used for identifiers and other purposes.
   */
  STRING
};

/**
 * @brief Interface representing a type in the RTTI system. This class provides information about the type, such as its
 * name, size, alignment, and kind. It also defines virtual functions for assigning values, constructing and destructing
 * instances of the type.
 */
class IType {
 public:
  explicit IType(const IString& name, std::size_t size, TypeKind kind) noexcept;

  explicit IType(const IString& name, std::size_t size, std::size_t alignment, TypeKind kind) noexcept;

  [[nodiscard]] std::size_t size() const noexcept;

  [[nodiscard]] std::size_t alignment() const noexcept;

  [[nodiscard]] TypeKind kind() const noexcept;

  [[nodiscard]] IString name() const noexcept;

  virtual ~IType() = default;

  virtual bool assign(const IType* srcType, void* dst, void* src) const = 0;

  [[nodiscard]] void* create() const;

  void destroy(void* memory) const noexcept;

  virtual bool construct(void* memory) const noexcept = 0;

  virtual void destruct(void* memory) const noexcept = 0;

  bool operator==(const IType& type) const noexcept;

  bool operator!=(const IType& type) const noexcept;

 private:
  const IString& m_name;
  const std::size_t m_size;
  const std::size_t m_alignment;
  const TypeKind m_kind;
};

template <typename T>
class TType : public IType {
 public:
  using Type = T;

  explicit TType(const IString& name, const TypeKind kind) : IType(name, sizeof(T), alignof(T), kind) {}

  virtual bool assign(T* destination, const T* source) const = 0;
};

class IClass : public IType {
 public:
  explicit IClass(const IString& name, std::size_t size) noexcept;

  explicit IClass(const IString& name, std::size_t size, std::size_t alignment) noexcept;
};
}  // namespace core::rtti
