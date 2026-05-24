#pragma once

#include <type_traits>

#include "Specialization.hpp"
#include "TypeRegistrar.hpp"
#include "rtti/TType.hpp"
#include "rtti/TypeRegistry.hpp"

namespace core::rtti {
template <typename A>
concept HasAutoDefinition = requires(A, IClassType* T) {
  { A::define(T) };
};

/**
 * @brief A template class that serves as a base for fundamental type registration in the RTTI system. This class is
 * intended to be specialized for specific fundamental types, where the specialization will define the necessary logic
 * for declaring and defining the fundamental type information in the TypeRegistry.
 *
 * This type must satisfy the following constraints:
 * - It must be a fundamental type (e.g., int, float, char, etc.).
 * - It must not be void.
 * - It must not be a nullptr_t.
 *
 * @tparam T The fundamental type for which to create the fundamental definition.
 */
template <typename T>
  requires(std::is_fundamental_v<std::remove_cvref_t<T>> && !std::is_void_v<std::remove_cvref_t<T>> &&
           !std::is_same_v<std::remove_cvref_t<T>, std::nullptr_t>)
class IFundamentalDefinition {
 public:
  /**
   * @brief Converts the IFundamentalDefinition to a Specialization based on the name of the fundamental type @code
   * T@endcode.
   */
  constexpr operator Specialization() const noexcept {
    return Specialization::of<T>();
  }

 private:
  /**
   * @brief A concrete implementation of the IType interface for fundamental types.
   */
  class FundamentalType : public TType<T> {
   public:
    /**
     * @brief Constructs a TypeImpl for the fundamental type T with its registered name.
     */
    FundamentalType() : TType<T>(GetTypeName<T>, TypeKind::FUNDAMENTAL) {}

    /**
     * @brief Virtual destructor for the TypeImpl class.
     */
    ~FundamentalType() override = default;
  };

  /**
   * @brief Declares the fundamental type information for the fundamental type @c T in the @code TypeRegistry@endcode.
   *
   * @param registry A non-const pointer to the @c TypeRegistry.
   */
  static void declare(TypeRegistry* registry) {
    registry->registerType(std::make_unique<FundamentalType>());
  }

  /**
   * @brief Defines the fundamental type information for the fundamental type @c T in the @code TypeRegistry@endcode.
   */
  static inline TypeRegistrar s_registrar{&declare};
};

/**
 * @brief A template class that serves as a base for class type registration in the RTTI system. This class is intended
 * to be specialized for specific class types, where the specialization will define the necessary logic for declaring
 * and defining the class type information in the TypeRegistry.
 *
 * @tparam T The class type for which to create the class definition.
 */
template <typename T>
  requires std::is_class_v<T>
class IClassDefinition {
 public:
  /**
   * @brief A type alias for the AutoRegistration specialization that will be used to register the class type
   * information for the class type @c T.
   */
  using RegistrationContainer = AutoRegistration<Specialization::of<T>()>;

  /**
   * @brief Converts the IClassDefinition to a Specialization based on the name of the class type @code T@endcode.
   *
   * This conversion operator is compile-time compatible, which allows instances of the class to be used as a template
   * type. This is very powerful, as it allows instances of the definition class to be created as template parameters
   * instead of global variables, and thus their static, automatic registration with the @c TypeRegistrar can occur.
   */
  constexpr operator Specialization() const noexcept {
    return Specialization::of<T>();
  }

 private:
  /**
   * @brief Declares the class type information for the class type @c T in the @c TypeRegistry.
   *
   * This method will be called during the declaration phase of type registration, and it should perform the necessary
   * logic to declare the class type in the RTTI system.
   *
   * @param registry A non-const pointer to the @c TypeRegistry, which can be used to register the class type
   * information.
   */
  static void declare(TypeRegistry* registry) {
    s_classType = registry->registerType(std::make_unique<IClassType>(GetTypeName<T>, TypeKind::CLASS));
  }

  /**
   * @brief Defines the class type information for the @c T class type in the @c TypeRegistry. This method will be
   * called during the definition phase of type registration, and it should perform the necessary logic to define the
   * class type in the RTTI system, including any properties, parent relationships, etc.
   */
  static void define() {
    if (s_classType) {
      if constexpr (HasAutoDefinition<RegistrationContainer>) {
        RegistrationContainer::define(s_classType);
      }
    }
  }

  /**
   * @brief Static instance of the @c TypeRegistrar that will trigger the registration of the class type information for
   * @c T when the class is loaded.
   */
  static inline TypeRegistrar s_registrar{&declare, &define};

  /**
   * @brief A static pointer to the IClassType information for the class type @c T that will be populated after
   * declaration with the @code TypeRegistry@endcode.
   */
  static inline IClassType* s_classType = nullptr;
};

}  // namespace core::rtti
