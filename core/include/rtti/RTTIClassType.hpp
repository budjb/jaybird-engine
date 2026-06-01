#pragma once

#include <span>

#include "Export.hpp"
#include "NamePool.hpp"
#include "RTTIClassFunction.hpp"
#include "RTTIName.hpp"
#include "RTTIProperty.hpp"
#include "RTTIType.hpp"

namespace core::rtti {
/**
 * @brief Polymorphic interface for class type descriptors in the RTTI system.
 *
 * Concrete class-type descriptors derive from this interface, while shared implementation lives in @code
 * TypedRTTIType@endcode.
 */
class JAYBIRD_API RTTIClassType : public RTTIType {
 public:
  /**
   * @brief Constructs an @c RTTIClassType with the given metadata.
   *
   * @param name The name of the class type, represented as a @code Name@endcode.
   * @param size The size of the class type in bytes.
   * @param alignment The alignment requirement of the class type in bytes.
   * @param isTrivial A boolean indicating whether the class type is trivially copyable.
   */
  explicit RTTIClassType(const Name& name, std::size_t size, std::size_t alignment, bool isTrivial) noexcept;

  /**
   * @brief Virtual destructor for the @c RTTIClassType interface.
   */
  ~RTTIClassType() override = default;

  /**
   * @brief Returns whether the class type is trivially copyable.
   *
   * @return @c true if the class type is trivially copyable, @c false otherwise.
   */
  [[nodiscard]] bool isTrivial() const noexcept;

  /**
   * @brief Adds a property to the class type descriptor.
   *
   * @param property A shared pointer to an @c RTTIProperty object representing the property to be added.
   */
  void property(std::shared_ptr<RTTIProperty>&& property) noexcept;

  /**
   * @brief Adds a property to the class type descriptor.
   *
   * @param property A shared pointer to an @c RTTIProperty object representing the property to be added.
   */
  void property(const std::shared_ptr<RTTIProperty>& property) noexcept;

  /**
   * @brief Retrieves a property from the class type descriptor by its name.
   *
   * @param name The name of the property to retrieve, represented as a @code Name@endcode.
   * @return A shared pointer to the @c RTTIProperty object if found, or @c nullptr if no property with the given name
   * exists.
   */
  [[nodiscard]] std::shared_ptr<RTTIProperty> property(const Name& name) const noexcept;

  /**
   * @brief Retrieves all properties of the class type descriptor.
   *
   * @return An unordered map containing all properties of the class type descriptor, where the keys are property names
   * represented as @code Name@endcode and the values are shared pointers to the corresponding @c RTTIProperty objects.
   */
  [[nodiscard]] std::unordered_map<Name, std::shared_ptr<RTTIProperty>> properties() const noexcept;

  /**
   * @brief Adds a member function to the class type descriptor.
   *
   * @param function A shared pointer to an @c RTTIClassFunction object representing the member function to be added.
   */
  void function(std::shared_ptr<RTTIClassFunction>&& function) noexcept;

  /**
   * @brief Adds a member function to the class type descriptor.
   *
   * @param function A shared pointer to an @c RTTIClassFunction object representing the member function to be added.
   */
  void function(const std::shared_ptr<RTTIClassFunction>& function) noexcept;

  /**
   * @brief Retrieves a member function from the class type descriptor by its name.
   *
   * @param name The name of the member function to retrieve, represented as a @code Name@endcode.
   * @return A shared pointer to the @c RTTIClassFunction object if found, or @c nullptr if no member function with the
   * given name exists.
   */
  [[nodiscard]] std::shared_ptr<RTTIClassFunction> function(const Name& name) const noexcept;

  /**
   * @brief Retrieves all member functions of the class type descriptor.
   *
   * @return An unordered map containing all member functions of the class type descriptor, where the keys are function
   * names represented as @code Name@endcode and the values are shared pointers to the corresponding @c
   * RTTIClassFunction objects.
   */
  [[nodiscard]] std::unordered_map<Name, std::shared_ptr<RTTIClassFunction>> functions() const noexcept;

 private:
  /**
   * @brief A boolean indicating whether the class type is trivially copyable.
   */
  bool m_trivial;

  /**
   * @brief Mutex to protect concurrent access to the properties of the class type.
   */
  mutable std::shared_mutex m_propertiesMutex;

  /**
   * @brief Mutex to protect concurrent access to the member functions of the class type.
   */
  mutable std::shared_mutex m_functionsMutex;

  /**
   * @brief A vector of @c RTTIProperty objects representing the properties of the class.
   */
  std::unordered_map<Name, std::shared_ptr<RTTIProperty>> m_properties;

  /**
   * @brief A vector of @c RTTIClassFunction objects representing the member functions of the class.
   */
  std::unordered_map<Name, std::shared_ptr<RTTIClassFunction>> m_functions;
};

/**
 * @brief A template class representing a class/struct type in the RTTI system.
 *
 * @tparam T The underlying class type that this @c TClassType represents.
 */
template <typename T>
  requires std::is_class_v<T>
class TypedRTTIClassType : public TypedRTTIType<T, RTTIClassType> {
 public:
  /**
   * @brief Defines a type alias for the underlying type @code T@endcode.
   */
  using Type = T;

  /**
   * @brief Constructs a @c TClassType for the specified type @code T@endcode.
   */
  explicit TypedRTTIClassType() : TypedRTTIType<T, RTTIClassType>(NamePool::get().addName(GetRTTIName<T>())) {}
};
}  // namespace core::rtti
