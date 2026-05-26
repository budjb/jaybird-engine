#pragma once

#include "IName.hpp"
#include "Export.hpp"
#include "TypeKind.hpp"

namespace core::rtti {
class IArrayType;

/**
 * @brief Interface representing a type in the RTTI system. This class provides information about the type, such as its
 * name, size, alignment, and kind. It also defines virtual functions for assigning values, constructing and destructing
 * instances of the type.
 */
class JAYBIRD_API IType {
 public:
  /**
   * @brief Constructs an IType with the given name, size, and kind. The alignment is set to maximum alignment by
   * default.
   *
   * @param name The name of the type, represented as an IName.
   * @param size The size of the type in bytes.
   * @param kind The kind of the type, represented as a TypeKind enumeration value.
   */
  explicit IType(const IName& name, std::size_t size, TypeKind kind) noexcept;

  /**
   * @brief Constructs an IType with the given name, size, alignment, and kind.
   *
   * @param name The name of the type, represented as an IName.
   * @param size The size of the type in bytes.
   * @param alignment The alignment requirement of the type in bytes.
   * @param kind The kind of the type, represented as a TypeKind enumeration value.
   */
  explicit IType(const IName& name, std::size_t size, std::size_t alignment, TypeKind kind) noexcept;

  /**
   * @brief Virtual destructor for the IType interface. This allows for proper cleanup of derived classes when deleting
   * through a pointer to IType.
   */
  virtual ~IType() = default;

  /**
   * @brief Returns the size of the type in bytes.
   *
   * @return The size of the type in bytes.
   */
  [[nodiscard]] std::size_t size() const noexcept;

  /**
   * @brief Returns the alignment requirement of the type in bytes.
   *
   * @return The alignment requirement of the type in bytes.
   */
  [[nodiscard]] std::size_t alignment() const noexcept;

  /**
   * @brief Returns the kind of the type, represented as a TypeKind enumeration value.
   *
   * @return The kind of the type, represented as a TypeKind enumeration value.
   */
  [[nodiscard]] TypeKind kind() const noexcept;

  /**
   * @brief Returns the name of the type, represented as an IName.
   *
   * @return The name of the type, represented as an IName.
   */
  [[nodiscard]] IName name() const noexcept;

  /**
   * @brief Assigns the value from the source pointer to the destination pointer. The actual assignment logic is defined
   * in derived classes, as it may involve complex copying or move semantics depending on the type.
   *
   * The destination pointer should point to a valid memory location that can hold an instance of the type, and the
   * source pointer should point to a valid instance of the type. The behavior is undefined if either pointer is null or
   * if they do not point to valid instances of the type.
   *
   * @param dst The destination pointer where the value should be assigned.
   * @param src The source pointer from which the value should be assigned.
   */
  virtual void assign(void* dst, const void* src) = 0;

  /**
   * @brief Allocates memory for an instance of the type. The actual allocation logic is defined in derived classes, as
   * it may involve specific memory management strategies depending on the type.
   *
   * The type is not constructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The caller is responsible for managing the allocated memory and ensuring that it is properly freed when no longer
   * needed.
   *
   * @return A pointer to the allocated memory for an instance of the type.
   */
  virtual void* allocate() = 0;

  /**
   * @brief Frees the memory allocated for an instance of the type. The actual deallocation logic is defined in derived
   * classes, as it may involve specific memory management strategies depending on the type.
   *
   * The type is not destructed as part of this allocation process; it only allocates raw memory that can hold an
   * instance of the type.
   *
   * The provided memory pointer should have been allocated by the allocate() function of this type, and it is the
   * caller's responsibility to ensure that the memory is properly freed when no longer needed.
   *
   * @param ptr A pointer to the memory that should be freed.
   */
  virtual void deallocate(void* ptr) = 0;

  /**
   * @brief Constructs an instance of the type in the provided memory location. The memory should be pre-allocated and
   * large enough to hold an instance of the type.
   *
   * The behavior is undefined if the memory pointer does not point to a valid memory location that can hold an instance
   * of the type.
   *
   * @param memory A pointer to the memory where the instance should be constructed. This memory should be pre-allocated
   * and large enough to hold an instance of the type.
   */
  virtual void construct(void* memory) noexcept = 0;

  /**
   * @brief Destructs an instance of the type located at the given memory location.
   *
   * The behavior is undefined if the memory pointer does not point to a valid instance of the type or was already
   * destructed.
   *
   * @param memory A pointer to the memory where the instance should be destructed.
   */
  virtual void destruct(void* memory) noexcept = 0;

  /**
   * @brief Creates a new instance of the type by allocating the required memory and constructing the object using its
   * default constructor.
   *
   * It is the caller's responsibility to manage the memory and ensure that it is properly freed when no longer needed.
   *
   * @return A pointer to the allocated memory containing a default-constructed instance of the type.
   */
  virtual void* create() = 0;

  /**
   * @brief Destroys the provided instance by calling its destructor and deallocating its memory.
   *
   * The memory pointed to by the parameter should have been allocated by the create() function of this type, and it is
   * the caller's responsibility to ensure that the memory is properly freed when no longer needed.
   *
   * @param memory A pointer to the memory that should be freed.
   */
  virtual void destroy(void* memory) = 0;

  /**
   * @brief Compares two instances of the type for equality. The behavior is undefined if either pointer does not point
   * to valid instances of the type.
   *
   * @param lhs A pointer to the first instance of the type to compare.
   * @param rhs A pointer to the second instance of the type to compare.
   * @return true if the instances are considered equal according to the type's equality semantics, false otherwise.
   */
  virtual bool equals(const void* lhs, const void* rhs) const noexcept = 0;

  /**
   * @brief Returns a pointer to the associated array type of this type.
   *
   * @return A pointer to the associated array type of this type.
   */
  [[nodiscard]] IArrayType* asArray() const noexcept;

  /**
   * @brief Compares this type with another type for equality.
   *
   * @param type The type to compare with this type.
   * @return true if the types are considered equal, false otherwise.
   */
  bool operator==(const IType& type) const noexcept;

  /**
   * @brief Compares this type with another type for inequality.
   *
   * @param type The type to compare with this type.
   * @return true if the types are considered not equal, false otherwise.
   */
  bool operator!=(const IType& type) const noexcept;

 private:
  /**
   * @brief The name of the type, represented as an IName. This is used to uniquely identify the type within the RTTI
   * system and can be used for type lookup and comparison.
   */
  const IName m_name;

  /**
   * @brief The size of the type in bytes.
   */
  const std::size_t m_size;

  /**
   * @brief The alignment requirement of the type in bytes.
   */
  const std::size_t m_alignment;

  /**
   * @brief The kind of the type, represented as a TypeKind enumeration value.
   */
  const TypeKind m_kind;
};
}  // namespace core::rtti
