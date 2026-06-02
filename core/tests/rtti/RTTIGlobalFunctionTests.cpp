#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "rtti/RTTIName.hpp"
#include "rtti/RTTISystem.hpp"
#include "rtti/RTTITypedFunction.hpp"

namespace test {
std::int32_t globalAnswer() {
  return 7;
}

std::int32_t globalSum(const std::int32_t lhs, const std::int32_t rhs) {
  return lhs + rhs;
}

void globalVoid() {}

struct UnregisteredArg {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::int32_t value;
};

UnregisteredArg globalReturnsUnregistered() {
  return UnregisteredArg{123};
}

void globalTakesUnregisteredArg(const UnregisteredArg) {}
}  // namespace test

REGISTER_TYPE_NAME(test::UnregisteredArg, "GlobalFunctionTests.UnregisteredArg");

static_assert(requires { core::rtti::TypedRTTIGlobalFunction("ok", &test::globalSum, "lhs", "rhs"); });
static_assert(!std::is_constructible_v<core::rtti::TypedRTTIGlobalFunction<decltype(&test::globalSum)>,
                                       const core::Name&, decltype(&test::globalSum), const char*>);

TEST_CASE(
    "Given a global function with no return and no arguments, when invoked with a valid stack frame, then the "
    "call succeeds",
    "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalVoid", &test::globalVoid);
  auto frame = function.createStackFrame();

  REQUIRE_NOTHROW(function.invoke(frame));
}

TEST_CASE(
    "Given a global function with a return value and no arguments, when invoked with a valid return pointer, "
    "then the return value is written",
    "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalAnswer", &test::globalAnswer);
  auto frame = function.createStackFrame();
  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 7);
}

TEST_CASE(
    "Given a global function with two arguments, when invoked with valid argument pointers, then both "
    "arguments are forwarded in order",
    "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalSum", &test::globalSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 10;
  std::int32_t rhs = 25;
  frame.argPtr(0, &lhs);
  frame.argPtr(1, &rhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 35);
}

TEST_CASE(
    "Given a global function with argument names and return value, when arguments are queried, then names "
    "and types match the reflected signature",
    "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalSum_metadata", &test::globalSum, "lhs", "rhs");
  auto args = function.arguments();
  auto* intType = core::rtti::RTTISystem::get().registry().getType(core::rtti::GetRTTIName<std::int32_t>());

  REQUIRE(args.size() == 2);
  REQUIRE(std::string(args[0]->name()) == "lhs");
  REQUIRE(std::string(args[1]->name()) == "rhs");
  REQUIRE(args[0]->type() == intType);
  REQUIRE(args[1]->type() == intType);
}

TEST_CASE(
    "Given a global function with a return type, when invoke is called without a return pointer, then an "
    "exception is thrown",
    "[rtti][global_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalAnswer_missing_return", &test::globalAnswer);
  auto frame = function.createStackFrame();

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a global function with arguments, when one argument pointer is missing, then invocation throws",
          "[rtti][global_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalSum_missing_arg", &test::globalSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 10;
  frame.argPtr(0, &lhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a global function with an unregistered return type, when reflected, then construction throws",
          "[rtti][global_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(core::rtti::TypedRTTIGlobalFunction("globalReturnsUnregistered", &test::globalReturnsUnregistered),
                    std::runtime_error);
}

TEST_CASE("Given a global function with an unregistered argument type, when reflected, then construction throws",
          "[rtti][global_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(
      core::rtti::TypedRTTIGlobalFunction("globalTakesUnregistered", &test::globalTakesUnregisteredArg, "arg"),
      std::runtime_error);
}

TEST_CASE("Given a global function, when the function name is queried, then it returns the correct name",
          "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::TypedRTTIGlobalFunction("test_name", &test::globalAnswer);

  REQUIRE(std::string(function.name()) == "test_name");
}

TEST_CASE("Given a global function, when return type is queried, then it corresponds to the actual return type",
          "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::TypedRTTIGlobalFunction("globalAnswer_returntype", &test::globalAnswer);
  auto* intType = core::rtti::RTTISystem::get().registry().getType(core::rtti::GetRTTIName<std::int32_t>());

  REQUIRE(function.returnType() == intType);
}

TEST_CASE("Given a global function, when flags are queried, then isMember is false and isNative is true",
          "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::TypedRTTIGlobalFunction("globalAnswer_flags", &test::globalAnswer);
  auto [isNative, isStatic] = function.flags();

  REQUIRE(isStatic);
  REQUIRE(isNative);
}

TEST_CASE(
    "Given a global function with arguments and return value, when operator() is used, then invocation succeeds and "
    "writes the return value",
    "[rtti][global_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalSum_callop", &test::globalSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 12;
  std::int32_t rhs = 8;
  std::int32_t result = 0;

  frame.argPtr(0, &lhs);
  frame.argPtr(1, &rhs);
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function(frame));
  REQUIRE(result == 20);
}

TEST_CASE(
    "Given a global function with a return type, when operator() is used without a return pointer, then an exception "
    "is thrown",
    "[rtti][global_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::TypedRTTIGlobalFunction("globalAnswer_callop_missing_return", &test::globalAnswer);
  auto frame = function.createStackFrame();

  REQUIRE_THROWS_AS(function(frame), std::runtime_error);
}
