#pragma once

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "IName.hpp"
#include "IType.hpp"
#include "JaybirdCoreExport.hpp"

namespace core::rtti {

/**
 * @brief Forward declaration of the IClassType interface, which represents class type information in the RTTI system.
 */
class IClassType;

/**
 * @brief The TypeRegistry class is a singleton that manages the registration and retrieval of type information in the
 * RTTI system. It provides methods for registering new types, retrieving type information by name, and checking for the
 * existence of types in the registry. The registry uses a thread-safe design with a shared mutex to allow for
 * concurrent access from multiple threads.
 *
 * Once registered, types are guaranteed to be available for retrieval and use throughout the application, making the
 * TypeRegistry a central component of the RTTI system. It allows for dynamic type information to be associated with
 * types at runtime, enabling features such as dynamic casting, type introspection, and more flexible type handling in
 * the application.
 */
class JAYBIRD_EXPORT TypeRegistry {
 public:
  /**
   * @brief Gets the singleton instance of the TypeRegistry. This is the main entry point for accessing the type
   * registry, and ensures that there is only one instance of the registry throughout the application.
   *
   * @return TypeRegistry* A pointer to the singleton instance of the TypeRegistry.
   */
  static TypeRegistry* get();

  /**
   * @brief Gets the type information for a given type name.
   *
   * @param name The name of the type to look up. This should be an IName that has been registered in the type registry.
   * @return IType* A pointer to the IType information for the given type name, or nullptr if the type is not found in
   * the registry.
   */
  IType* getType(const IName& name) const;

  /**
   * @brief Gets the class type information for a given type name. This is a convenience method that checks if the type
   * is a class type and returns it as an IClassType if it is.
   *
   * @param name The name of the type to look up. This should be an IName that has been registered in the type registry.
   * @return IClassType* A pointer to the IClassType information for the given type name if it is a class type, or
   * nullptr if the type is not found in the registry or is not a class type.
   */
  IClassType* getClass(const IName& name) const;

  /**
   * @brief Registers a new type in the type registry. This method takes ownership of the provided IType and adds it to
   * the registry.
   *
   * @param type A unique pointer to the IType to register. The type's name will be used as the key in the registry, so
   * it must be unique.
   * @return IType* A pointer to the registered IType if the registration was successful, or nullptr if a type with the
   * same name already exists in the registry.
   */
  IType* registerType(std::unique_ptr<IType>&& type);

  /**
   * @brief Checks if a type with the given name exists in the type registry. This is a convenience method that allows
   * for quick checks for the existence of a type without needing to retrieve the full type information.
   *
   * @param name The name of the type to check for. This should be an IName that may have been registered in the type
   * registry.
   * @return bool True if a type with the given name exists in the registry, or false if it does not.
   */
  bool hasType(const IName& name) const noexcept;

 private:
  /**
   * @brief Constructs a new Type Registry object. This is private to enforce the singleton pattern.
   */
  TypeRegistry() = default;

  /**
   * @brief A mutex to protect access to the type registry. This allows for thread-safe access to the registry.
   */
  mutable std::shared_mutex m_mutex;

  /**
   * @brief A map of type names to their corresponding type information. This is the core of the type registry, allowing
   * for fast lookups of types by name.
   */
  std::unordered_map<IName, std::unique_ptr<IType>> m_types;
};
}  // namespace core::rtti
