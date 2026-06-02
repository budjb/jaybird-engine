#include "rtti/RTTIGlobalFunction.hpp"

namespace {
using namespace core::rtti;

}  // namespace
namespace core::rtti {
RTTIGlobalFunction::RTTIGlobalFunction(const std::string_view name, const FunctionFlags flags) noexcept
    : RTTIFunction(name, flags) {
  m_flags.isStatic = true;
}

RTTIGlobalFunction::~RTTIGlobalFunction() = default;
}  // namespace core::rtti
