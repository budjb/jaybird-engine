#pragma once

#include "Hash.hpp"
#include "TypeName.hpp"

namespace core::rtti {
/**
 * @brief The Specialization class provides a way to create compile-time, polymorphic template uniqueness via an FNV-1a
 * hash. This allows the creation of arbitrary uniqueness constraints that are not tied class types alone.
 */
class Specialization {
 public:
  /**
   * @brief Constructs a Specialization from a string view. The constructor takes a string view and computes its FNV-1a
   * hash to create a unique identifier for the specialization. This allows for compile-time uniqueness based on string
   * values, which can be used in template specializations to create unique types based on string identifiers.
   *
   * @param str The string view to use for creating the specialization. The FNV-1a hash of this string will be computed
   * and stored as the unique identifier for the specialization.
   * @note The string view should be a compile-time constant for the specialization to be usable in compile-time
   * contexts, such as template specializations.
   */
  explicit constexpr Specialization(const std::string_view str) noexcept : m_hash(fnv1a_64(str)) {}

  /**
   * @brief Creates a Specialization for a given type T. This static method checks if the type T has a static member
   * variable named NAME that can be used as a type name, or if there is a specialization of the TypeName struct for T
   * that defines a value member variable. If either of these conditions is satisfied, it uses the corresponding string
   * as the basis for creating the Specialization. If neither condition is satisfied, it triggers a static assertion
   * failure, indicating that the type T does not have a valid name member or type name mapping for creating a
   * Specialization.
   *
   * @tparam T The type for which to create the Specialization.
   * @return A Specialization object created based on the name of the type T.
   */
  template <typename T>
    requires HasTypeName<T>
  constexpr static Specialization of() noexcept {
    return Specialization(GetTypeName<T>());
  }

  /**
   * @brief Converts the Specialization to its underlying hash value. This allows the Specialization to be used in
   * contexts where a hash value is needed, such as in template specializations or as keys in compile-time maps. The
   * conversion operator returns the FNV-1a hash that represents the unique identifier for the specialization.
   */
  hash_t m_hash;
};

}  // namespace core::rtti
