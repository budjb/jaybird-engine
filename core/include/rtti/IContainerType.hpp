#pragma once
#include "IType.hpp"

namespace core::rtti {
/**
 * @brief Interface for type descriptors representing containers that encapsulate other types (e.g., arrays, maps, smart
 * pointers, etc.).
 */
class IContainerType : public IType {
 public:
  /**
   * @brief Constructs an IContainerType with the given name, size, alignment, and inner type descriptor.
   *
   * @param name The interned string name of the container type.
   * @param size The size of the container type in bytes.
   * @param alignment The alignment requirement of the container type in bytes.
   * @param inner A pointer to the IType descriptor for the type contained within the container.
   */
  explicit IContainerType(const IName& name, std::size_t size, std::size_t alignment, const IType* inner) noexcept;

  /**
   * @brief Virtual destructor for IContainerType. This allows for proper cleanup of derived classes when deleting
   * through a pointer to IContainerType.
   */
  ~IContainerType() override = default;

  /**
   * @brief Returns a pointer to the IType descriptor for the type contained within the container. The returned pointer
   * is guaranteed to be valid for the lifetime of the container type descriptor and should not be modified by the
   * caller.
   *
   * @return A pointer to the IType descriptor for the type contained within the container.
   */
  [[nodiscard]] const IType* inner() const noexcept;

 private:
  /**
   * @brief A pointer to the IType descriptor for the type contained within the container.
   */
  const IType* m_inner;
};
}  // namespace core::rtti
