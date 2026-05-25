#include "rtti/registration/TypeRegistrar.hpp"

#include <algorithm>

#include "rtti/TypeRegistry.hpp"

namespace core::rtti {
std::vector<TypeRegistrar::DeclareFunction> TypeRegistrar::s_declareFunctions;
std::vector<TypeRegistrar::DefineFunction> TypeRegistrar::s_definitionFunctions;

TypeRegistrar::TypeRegistrar(const DeclareFunction& declare) {
  s_declareFunctions.push_back(declare);
}

TypeRegistrar::TypeRegistrar(const DeclareFunction& declare, const DefineFunction& define) {
  s_declareFunctions.push_back(declare);
  s_definitionFunctions.push_back(define);
}

void TypeRegistrar::registerTypes() {
  auto* registry = TypeRegistry::get();

  std::ranges::for_each(s_declareFunctions, [&registry](const DeclareFunction& function) { function(registry); });
  std::ranges::for_each(s_definitionFunctions, [](const DefineFunction& function) { function(); });
}
}  // namespace core::rtti
