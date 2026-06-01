#pragma once

#include <concepts>
#include <memory>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>

#include "Export.hpp"
#include "RTTIArrayType.hpp"
#include "RTTIName.hpp"
#include "RTTIRefType.hpp"
#include "RTTIType.hpp"
#include "RTTIWeakRefType.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief Forward declaration of the RTTIClassType interface, which represents class type information in the RTTI
 * system.
 */
class RTTIClassType;

/**
 * @brief Concept that checks whether a type @c T exposes a nested @c Type member alias.
 *
 * @tparam T The type to check for the presence of a nested @c Type member.
 */
template <typename T>
constexpr bool has_type_v = requires { typename T::Type; };

/**
 * @brief Concept that identifies RTTI type descriptors that represent container types.
 *
 * @tparam T The candidate descriptor type to check.
 */
template <typename T>
constexpr bool is_container_type_v = std::derived_from<T, RTTIContainerType>;

/**
 * @brief Concept that identifies a concrete RTTI type descriptor suitable for registration.
 *
 * A type @c D satisfies @c TypeDescriptor if it publicly derives from @c RTTIType.
 *
 * @tparam D The candidate descriptor type to check.
 */
template <typename D>
concept TypeDescriptor = std::derived_from<D, RTTIType>;

/**
 * @brief A singleton that manages registration and retrieval of type information.
 *
 * The registry provides methods for registering types, retrieving type information by name, and checking for type
 * existence. It uses a thread-safe design with a shared mutex to allow concurrent access. Once registered, types are
 * guaranteed to be available throughout the application's lifetime, enabling features like type introspection and
 * dynamic casting.
 *
 * Types are accessed through @c RTTISystem::get().registry() rather than a direct singleton on this class.
 */
class JAYBIRD_API RTTIRegistry {
 public:
  /**
   * @brief Retrieves the type descriptor for a given type name.
   *
   * @param name The @c Name of the type to look up.
   * @return A pointer to the @c RTTIType descriptor, or @c nullptr if no type with that name is registered.
   */
  RTTIType* getType(const Name& name) const;

  /**
   * @brief Retrieves the class type descriptor for a given type name.
   *
   * This is a convenience method that returns @c nullptr both when the type is not found and when it
   * is found but is not a class type.
   *
   * @param name The @c Name of the type to look up.
   * @return A pointer to the @c RTTIClassType descriptor, or @c nullptr if the type is not found or is not a class
   * type.
   */
  RTTIClassType* getClass(const Name& name) const;

  /**
   * @brief Registers a type descriptor and, when applicable, automatically registers its corresponding
   * @c RTTIArrayTType<D::Type> as well.
   *
   * On success, the registry takes ownership of the descriptor. If a type with the same name is already registered,
   * the descriptor is discarded and @c false is returned — existing entries are never overwritten. When the element
   * type @c D::Type satisfies the @c HasTypeName concept, an @c RTTIArrayTType<D::Type> is also registered under the
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
   * @brief Unregisters a type descriptor by its name.
   *
   * This method removes the type descriptor associated with the given name from the registry. If the type is found and
   * successfully unregistered, it returns @c true; otherwise, it returns @c false. Note that unregistering a type that
   * has dependent types (e.g., an array type that depends on a fundamental type) may lead to dangling pointers in the
   * registry, so it should be used with caution.
   *
   * @param name The @c Name of the type to remove.
   * @return @c true if the type existed and was removed, or @c false if no matching type was registered.
   */
  bool unregisterType(const Name& name);

  /**
   * @brief Checks whether a type with the given name is registered.
   *
   * @param name The @c Name of the type to check.
   * @return @c true if a type with that name exists in the registry, @c false otherwise.
   */
  bool hasType(const Name& name) const noexcept;

 private:
  /**
   * @brief Inserts a type descriptor into the map without acquiring the mutex.
   *
   * This is the lock-free core of registration, called from @c registerType while the exclusive lock is already held.
   * It also handles compile-time-conditional auto-registration of the companion @c RTTIArrayTType<D::Type>: the
   * companion is only created when @c D::Type satisfies @c HasTypeName (so that @c RTTIArrayTType can derive its
   * canonical name at compile time). When that condition is false, no companion is created and no runtime recursion
   * occurs. This design avoids both deadlock on the non-recursive @c std::shared_mutex and spurious compiler
   * instantiation errors.
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
   * @brief A pointer to the singleton instance of the RTTIRegistry.
   */
  static RTTIRegistry* s_instance;

  /**
   * @brief A map of type names to their corresponding type information. This is the core of the type registry, allowing
   * for fast lookups of types by name.
   */
  std::unordered_map<Name, std::unique_ptr<RTTIType>> m_types;
};

template <TypeDescriptor D>
bool RTTIRegistry::registerType(std::unique_ptr<D>&& type) {
  std::unique_lock lock(m_mutex);
  return registerTypeImpl(std::move(type));
}

template <TypeDescriptor D>
bool RTTIRegistry::registerTypeImpl(std::unique_ptr<D>&& type) {
  auto* instance = type.get();

  if (auto [it, success] = m_types.insert({type->name(), std::move(type)}); success) {
    if constexpr (has_type_v<D>) {
      using NativeType = D::Type;

      if constexpr (!is_container_type_v<D> && NamedRTTIType<NativeType>) {
        if (instance->kind() == RTTITypeKind::CLASS) {
          auto refType = std::make_unique<RTTIRefTType<NativeType>>(instance);
          auto* refDescriptor = refType.get();

          registerTypeImpl(std::move(refType));
          registerTypeImpl(std::make_unique<RTTIWeakRefTType<NativeType>>(instance));
          registerTypeImpl(std::make_unique<RTTIArrayTType<std::shared_ptr<NativeType>>>(refDescriptor));
        } else {
          registerTypeImpl(std::make_unique<RTTIArrayTType<NativeType>>(instance));
        }
      }
    } else {
      // TODO: handle runtime-only companion types
    }
    return true;
  }

  return false;
}
}  // namespace core::rtti
