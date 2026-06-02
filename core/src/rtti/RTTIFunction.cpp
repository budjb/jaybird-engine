#include "rtti/RTTIFunction.hpp"

#include "rtti/RTTISystem.hpp"

namespace core::rtti {
RTTIFunction::RTTIFunction(const std::string_view name, const FunctionFlags flags) noexcept
    : m_name(NamePool::get().addName(name)), m_flags(flags) {}

const Name& RTTIFunction::name() const noexcept {
  return m_name;
}

FunctionFlags RTTIFunction::flags() const noexcept {
  return m_flags;
}

std::vector<const RTTIArgument*> RTTIFunction::arguments() const noexcept {
  std::vector<const RTTIArgument*> arguments;
  arguments.reserve(m_arguments.size());

  for (const auto& arg : m_arguments) {
    arguments.push_back(arg.get());
  }

  return arguments;
}

void RTTIFunction::argument(const std::string_view name, RTTIType* type) noexcept {
  m_arguments.push_back(std::make_unique<RTTIArgument>(NamePool::get().addName(name), type));
}

const RTTIType* RTTIFunction::returnType() const noexcept {
  return m_return;
}

void RTTIFunction::returnType(RTTIType* type) noexcept {
  m_return = type;
}

RTTIStackFrame RTTIFunction::createStackFrame() const noexcept {
  return RTTIStackFrame(m_arguments.size(), m_return != nullptr, m_flags.isStatic);
}

void RTTIFunction::operator()(RTTIStackFrame& frame) {
  invoke(frame);
}
}  // namespace core::rtti
