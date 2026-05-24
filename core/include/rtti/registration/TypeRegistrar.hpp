#pragma once

#include <vector>

namespace core::rtti {
/**
 * @brief Forward declaration of the TypeRegistry class.
 */
class TypeRegistry;

/**
 * @brief The @c TypeRegistrar class is responsible for managing the automatic registration of types in the RTTI system
 * via runtime discovery.
 *
 * It maintains two static vectors of function pointers: one for declaration functions and one for definition functions.
 * When a new type is registered, its corresponding declaration and definition functions are added to these vectors.
 *
 * Type registration is performed in two phases: first, all automatically-discovered types are declared to the RTTI
 * system, and then they are defined. Declaration only informs the RTTI system of the existence of a type and its name
 * with no other details (such as class properties, parent relationships, etc.). Definition provides the full details of
 * the type, and must be performed after all types have been declared to ensure that any inter-type relationships can be
 * properly resolved.
 *
 * Classes that utilize the @c TypeRegistrar for automatic type registration must define a @code static inline@endcode
 * instance of the
 * @c TypeRegistrar, which will trigger the registration process when the class is loaded. This instance should be
 * initialized with the appropriate declaration and definition functions for the type being registered.
 */
class TypeRegistrar {
 public:
  /**
   * @brief Defines the type of function pointers used for declaring types in the RTTI system.
   *
   * The declaration function is expected to return void and take a non-const pointer to the @c TypeRegistry as its
   * argument.
   */
  using DeclareFunction = void (*)(TypeRegistry*);

  /**
   * @brief Defines the type of function pointers used for defining types in the RTTI system.
   *
   * The definition function is expected to return void and take no arguments.
   */
  using DefineFunction = void (*)();

  /**
   * @brief Constructs a @c TypeRegistrar with the given declaration function. This constructor is
   * called when a new type is registered and only requires a declaration phase.
   *
   * @param declare The function pointer for the declaration function of the type being registered.
   */
  explicit TypeRegistrar(const DeclareFunction& declare);

  /**
   * @brief Constructs a @c TypeRegistrar with the given declaration and definition functions. This constructor is
   * called when a new type is registered and requires both declaration and definition phases.
   *
   * @param declare The function pointer for the declaration function of the type being registered.
   * @param define The function pointer for the definition function of the type being registered.
   */
  explicit TypeRegistrar(const DeclareFunction& declare, const DefineFunction& define);

  /**
   * @brief Registers all types that have been automatically discovered.
   *
   * This method will be called during the initialization phase of the application to ensure that all types are properly
   * registered in the RTTI system.
   *
   * The registration process is performed in two phases. First, all declaration functions are called to declare the
   * types to the RTTI system, which allows the system to be aware of the existence of the types and their names without
   * needing any details about the types themselves. Afterward, all definition functions are called to provide the full
   * details of the types, such as their properties, functions, parent relationships, etc.
   *
   * This two-phase approach ensures that any inter-type relationships can be properly resolved during the definition
   * phase, as all types will have been declared and their names will be known to the RTTI system before any definitions
   * are processed.
   */
  static void registerTypes();

 private:
  /**
   * @brief Static vector of function pointers for declaring types in the RTTI system. Each function in this vector is
   * expected to take a non-const pointer to the @c TypeRegistry and perform the necessary declaration logic for a
   * specific type.
   */
  static inline std::vector<DeclareFunction> s_declareFunctions;

  /**
   * @brief Static vector of function pointers for defining types in the RTTI system. Each function in this vector is
   * expected to take a non-const pointer to the @c TypeRegistry and perform the necessary definition logic for a
   * specific type.
   */
  static inline std::vector<DefineFunction> s_definitionFunctions;
};
}  // namespace core::rtti
