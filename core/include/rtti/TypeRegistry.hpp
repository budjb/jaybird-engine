#pragma once

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "Export.hpp"
#include "IName.hpp"
#include "IType.hpp"

namespace core::rtti {

/**
 * @brief Forward declaration of the IClassType interface, which represents class type information in the RTTI system.
 */
class IClassType;

/**
 * @brief A singleton that manages registration and retrieval of type information.
 *
 * The registry provides methods for registering types, retrieving type information by name, and checking for type
 * existence. It uses a thread-safe design with a shared mutex to allow concurrent access. Once registered, types are
 * guaranteed to be available throughout the application's lifetime, enabling features like type introspection and
 * dynamic casting.
 */
class JAYBIRD_API TypeRegistry {
 public:
  /**
   * @brief Retrieves the type descriptor for a given type name.
   *
   * @param name The @c IName of the type to look up.
   * @return A pointer to the @c IType descriptor, or @c nullptr if no type with that name is registered.
   */
  IType* getType(const IName& name) const;

  /**
   * @brief Retrieves the class type descriptor for a given type name.
   *
   * This is a convenience method that returns @c nullptr both when the type is not found and when it
   * is found but is not a class type.
   *
   * @param name The @c IName of the type to look up.
   * @return A pointer to the @c IClassType descriptor, or @c nullptr if the type is not found or is not a class type.
   */
  IClassType* getClass(const IName& name) const;

  /**
   * @brief Registers a new type in the registry, transferring ownership from the caller.
   *
   * If a type with the same name is already registered, the provided type is discarded and
   * @c nullptr is returned; no overwrite occurs.
   *
   * @param type The @c IType descriptor to register. The registry takes ownership.
   * @return A non-owning pointer to the registered @c IType, or @c nullptr if a type with that name already exists.
   */
  IType* registerType(std::unique_ptr<IType>&& type);

  /**
   * @brief Checks whether a type with the given name is registered.
   *
   * @param name The @c IName of the type to check.
   * @return @c true if a type with that name exists in the registry, @c false otherwise.
   */
  bool hasType(const IName& name) const noexcept;

 private:
  /**
   * @brief A flag indicating whether the type registry has been initialized.
   */
  bool m_initialized = false;

  /**
   * @brief A mutex to protect access to the type registry. This allows for thread-safe access to the registry.
   */
  mutable std::shared_mutex m_mutex;

  /**
   * @brief A pointer to the singleton instance of the TypeRegistry.
   */
  static TypeRegistry* s_instance;

  /**
   * @brief A map of type names to their corresponding type information. This is the core of the type registry, allowing
   * for fast lookups of types by name.
   */
  std::unordered_map<IName, std::unique_ptr<IType>> m_types;
};
}  // namespace core::rtti
