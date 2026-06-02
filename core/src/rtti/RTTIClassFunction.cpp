#include "rtti/RTTIClassFunction.hpp"

namespace core::rtti {
RTTIClassFunction::RTTIClassFunction(const std::string_view name, const FunctionFlags flags) noexcept
    : RTTIFunction(name, flags) {}

RTTIClassFunction::~RTTIClassFunction() {}
}  // namespace core::rtti
