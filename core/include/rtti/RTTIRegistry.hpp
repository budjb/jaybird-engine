#pragma once

#include <concepts>
#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "Export.hpp"
#include "RTTIArrayType.hpp"
#include "RTTIName.hpp"
#include "RTTIRefType.hpp"
#include "RTTIWeakRefType.hpp"
#include "rtti/RTTIGlobalFunction.hpp"
#include "rtti/RTTIType.hpp"
#include "types/Name.hpp"

namespace core::rtti {
/**
 * @brief This variable template is @c true when @c T exposes a nested @c Type member alias.
 *
 * @tparam T The type to check for the presence of a nested @c Type member.
 */
template <typename T>
constexpr bool has_type_v = requires { typename T::Type; };

/**
 * @brief This variable template is @c true when @c T publicly derives from @code RTTIContainerType@endcode.
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
   * @brief Destroys the registry and releases all owned type and function descriptors.
   */
  ~RTTIRegistry();

  /**
   * @brief Checks whether a type with the given name is registered.
   *
   * @param name The @c Name of the type to check.
   * @return This function returns @c true if a type with that name exists in the registry, and it returns @c false
   * otherwise.
   */
  bool hasType(const Name& name) const noexcept;

  /**
   * @brief Retrieves the type descriptor for a given type name.
   *
   * @param name The @c Name of the type to look up.
   * @return This function returns a pointer to the @c RTTIType descriptor, or it returns @code nullptr@endcode if no
   * type with that name is registered.
   */
  RTTIType* getType(const Name& name) const;

  /**
   * @brief Retrieves the class type descriptor for a given type name.
   *
   * This is a convenience method that returns @c nullptr both when the type is not found and when it
   * is found but is not a class type.
   *
   * @param name The @c Name of the type to look up.
   * @return This function returns a pointer to the @c RTTIClassType descriptor, or it returns
   * @code nullptr@endcode if the type is not found or is not a class type.
   */
  RTTIClassType* getClass(const Name& name) const;

  /**
   * @brief Registers a type descriptor and, when applicable, automatically registers its corresponding
   * @c TypedRTTIArrayType<D::Type> as well.
   *
   * On success, the registry takes ownership of the descriptor. If a type with the same name is already registered,
   * the descriptor is discarded and @code nullptr@endcode is returned, and existing entries are never overwritten. When
   * the element
   * type @c D::Type satisfies the @c HasTypeName concept, an @c TypedRTTIArrayType<D::Type> is also registered under
   * the canonical @c "array:<typename>" name. The entire operation (including the companion array registration) is
   * performed under a single exclusive lock, so it is atomic with respect to other registry operations.
   *
   * @tparam D The concrete descriptor type, which must satisfy the @c TypeDescriptor concept.
   * @param type An owning pointer to the descriptor to register; ownership is transferred on success.
   * @return This function returns a pointer to the registered descriptor when insertion succeeds, or it returns
   * @code nullptr@endcode when a type with the same name already exists.
   */
  template <TypeDescriptor D>
  D* registerType(std::unique_ptr<D>&& type) {
    std::unique_lock lock(m_typesMutex);
    return registerTypeImpl(std::move(type));
  }

#ifdef TESTING_ENABLED
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
#endif

  /**
   * @brief Checks whether a global function with the given name is registered in the registry.
   *
   * @param name The @c Name of the global function to check for existence.
   * @return @c true if a global function with the specified name is registered, @c false otherwise.
   */
  [[nodiscard]] bool hasFunction(const Name& name) const noexcept;

  /**
   * @brief Retrieves the global function descriptor associated with the given name.
   *
   * @param name The @c Name of the global function to retrieve.
   * @return A pointer to the @c RTTIGlobalFunction descriptor if found, or @c nullptr if no global function with the
   * specified name is registered.
   */
  [[nodiscard]] RTTIGlobalFunction* getFunction(const Name& name) const noexcept;

  /**
   * @brief Registers a global function descriptor in the registry.
   *
   * @param function An owning pointer to the @c RTTIGlobalFunction descriptor to register; ownership is transferred on
   * success.
   * @return A pointer to the registered @c RTTIGlobalFunction descriptor if registration was successful, or @c nullptr
   * if a global function with the same name already exists in the registry.
   */
  RTTIGlobalFunction* registerFunction(std::unique_ptr<RTTIGlobalFunction>&& function) noexcept;

#ifdef TESTING_ENABLED
  /**
   * @brief Unregisters a global function descriptor by its name.
   *
   * @param name The @c Name of the global function to remove from the registry.
   * @return @c true if a global function with the specified name existed and was successfully unregistered, or @c false
   * if no matching global function was found in the registry.
   */
  bool unregisterFunction(const Name& name) noexcept;
#endif

 private:
  /**
   * @brief Inserts a type descriptor into the map without acquiring the mutex.
   *
   * This is the lock-free core of registration, called from @c registerType while the exclusive lock is already held.
   * It also handles compile-time-conditional auto-registration of the companion @c TypedRTTIArrayType<D::Type>: the
   * companion is only created when @c D::Type satisfies @c HasTypeName (so that @c TypedRTTIArrayType can derive its
   * canonical name at compile time). When that condition is false, no companion is created and no runtime recursion
   * occurs. This design avoids both deadlock on the non-recursive @c std::shared_mutex and spurious compiler
   * instantiation errors.
   *
   * @tparam D The concrete descriptor type, which must satisfy the @c TypeDescriptor concept.
   * @param type An owning pointer to the descriptor to insert.
   * @return This function returns a pointer to the inserted descriptor when insertion succeeds, or it returns
   * @code nullptr@endcode when the name is already present.
   */
  template <TypeDescriptor D>
  D* registerTypeImpl(std::unique_ptr<D>&& type) {
    auto* instance = type.get();

    if (auto [it, success] = m_types.insert({type->name(), std::move(type)}); success) {
      if constexpr (has_type_v<D>) {
        using NativeType = D::Type;

        if constexpr (!is_container_type_v<D> && NamedRTTIType<NativeType>) {
          if (instance->kind() == RTTITypeKind::CLASS) {
            auto refType = std::make_unique<TypedRTTIRefType<NativeType>>(instance);
            auto* refDescriptor = refType.get();

            registerTypeImpl(std::move(refType));
            registerTypeImpl(std::make_unique<TypedRTTIWeakRefType<NativeType>>(instance));
            registerTypeImpl(std::make_unique<TypedRTTIArrayType<std::shared_ptr<NativeType>>>(refDescriptor));
          } else {
            registerTypeImpl(std::make_unique<TypedRTTIArrayType<NativeType>>(instance));
          }
        }
      } else {
        // TODO: handle runtime-only companion types
      }
      return instance;
    }

    return nullptr;
  }

  /**
   * @brief This flag indicates whether the type registry has been initialized.
   */
  bool m_initialized = false;

  /**
   * @brief This mutex protects access to the types map, enabling thread-safe reads and writes.
   */
  mutable std::shared_mutex m_typesMutex;

  /**
   * @brief This map stores RTTI type descriptors, keyed by their interned name.
   */
  std::unordered_map<Name, std::unique_ptr<RTTIType>> m_types;

  /**
   * @brief This mutex protects access to the global functions map, enabling thread-safe reads and writes.
   */
  mutable std::shared_mutex m_functionsMutex;

  /**
   * @brief This map stores global function descriptors, keyed by their interned name.
   */
  std::unordered_map<Name, std::unique_ptr<RTTIGlobalFunction>> m_functions;
};
}  // namespace core::rtti
