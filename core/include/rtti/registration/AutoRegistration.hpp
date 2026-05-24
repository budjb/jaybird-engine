#pragma once

#include "Specialization.hpp"

namespace core::rtti {

/**
 * @brief A template container class that serves as a base for type registration in the RTTI system. This class is
 * intended to be specialized for specific types or type categories, where the specialization will define the necessary
 * logic for declaring and defining the type information in the TypeRegistry.
 *
 * The AutoRegistration class works in conjunction with the TypeRegistrar to ensure that types are registered in the
 * correct order, with declarations happening before definitions. This allows for a flexible and extensible type
 * registration system where new types can be added by simply specializing this template with the appropriate logic for
 * declaration and definition.
 *
 * By itself, the class does nothing other than provide a structure to instantiate a specialization. However, it may
 * hold arbitrary properties or functions useful to the provided specialization when it runs.
 *
 * Here's an example of its usage combined with the @c IClassDefinition class, which automatically registers new class
 * types with the RTTI @code TypeRegistry@endcode:
 *
 * @code
 * template <>
 * class core::rtti::AutoRegistration<core::rtti::IClassDefinition<Foo>{}> {
 *   public:
 *     static void define(core::rtti::IClassType* type) {
 *     using T = _type;
 *     {
 *       // Class definition logic goes here
 *     }
 *   };
 * };@endcode
 */
template <Specialization>
class AutoRegistration;
}  // namespace core::rtti
