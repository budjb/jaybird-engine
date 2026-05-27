#pragma once

#include <concepts>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "ArrayType.hpp"
#include "Export.hpp"
#include "IName.hpp"
#include "IType.hpp"
#include "TypeName.hpp"

namespace core::rtti {

/**
 * @brief Forward declaration of the IClassType interface, which represents class type information in the RTTI system.
 */
class IClassType;

/**
 * @brief Concept that identifies a concrete RTTI type descriptor suitable for registration.
 *
 * A type @c D satisfies @c TypeDescriptor if it publicly derives from @c IType and
 * exposes a @c Type member alias that names the underlying C++ type it describes.
 *
 * @tparam D The candidate descriptor type to check.
 */
template <typename D>
concept TypeDescriptor = std::derived_from<D, IType> && requires { typename D::Type; };

/**
 * @brief A singleton that manages registration and retrieval of type information.
 *
 * The registry provides methods for registering types, retrieving type information by name, and checking for type
 * existence. It uses a thread-safe design with a shared mutex to allow concurrent access. Once registered, types are
 * guaranteed to be available throughout the application's lifetime, enabling features like type introspection and
 * dynamic casting.
 *
 * Types are accessed through @c TypeSystem::get().registry() rather than a direct singleton on this class.
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
   * @brief Registers a type descriptor and, when applicable, automatically registers its corresponding
   * @c TArrayType<D::Type> as well.
   *
   * On success, the registry takes ownership of the descriptor. If a type with the same name is already registered,
   * the descriptor is discarded and @c false is returned — existing entries are never overwritten. When the element
   * type @c D::Type satisfies the @c HasTypeName concept, a @c TArrayType<D::Type> is also registered under the
   * canonical @c "array:<typename>" name. The entire operation (including the companion array registration) is
   * performed under a single exclusive lock, so it is atomic with respect to other registry operations.
   *
   * @tparam D The concrete descriptor type, which must satisfy the @c TypeDescriptor concept.
   * @param type An owning pointer to the descriptor to register; ownership is transferred on success.
   * @return @c true if the type was successfully registered, @c false if a type with the same name already existed.
   */
  template <TypeDescriptor D>
  bool registerType(std::unique_ptr<D>&& type);

  /**
   * @brief Checks whether a type with the given name is registered.
   *
   * @param name The @c IName of the type to check.
   * @return @c true if a type with that name exists in the registry, @c false otherwise.
   */
  bool hasType(const IName& name) const noexcept;

 private:
  /**
   * @brief Inserts a type descriptor into the map without acquiring the mutex.
   *
   * This is the lock-free core of registration, called from @c registerType while the exclusive lock is already held.
   * It also handles compile-time-conditional auto-registration of the companion @c TArrayType<D::Type>: the companion
   * is only created when @c D::Type satisfies @c HasTypeName (so that @c TArrayType can derive its canonical name at
   * compile time). When that condition is false, no companion is created and no runtime recursion occurs. This design
   * avoids both deadlock on the non-recursive @c std::shared_mutex and spurious compiler instantiation errors.
   *
   * @tparam D The concrete descriptor type, which must satisfy the @c TypeDescriptor concept.
   * @param type An owning pointer to the descriptor to insert.
   * @return @c true if the descriptor was inserted, @c false if the name was already present.
   */
  template <TypeDescriptor D>
  bool registerTypeImpl(std::unique_ptr<D>&& type);

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

template <TypeDescriptor D>
bool TypeRegistry::registerType(std::unique_ptr<D>&& type) {
  std::unique_lock lock(m_mutex);
  return registerTypeImpl(std::move(type));
}

template <TypeDescriptor D>
bool TypeRegistry::registerTypeImpl(std::unique_ptr<D>&& type) {
  auto* instance = type.get();

  if (auto [it, success] = m_types.insert({type->name(), std::move(type)}); success) {
    if constexpr (HasTypeName<typename D::Type>) {
      registerTypeImpl(std::make_unique<TArrayType<typename D::Type>>(instance));
    }
    return true;
  }

  return false;
}
}  // namespace core::rtti
