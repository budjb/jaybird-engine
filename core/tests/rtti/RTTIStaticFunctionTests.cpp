#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "rtti/RTTIStaticFunction.hpp"
#include "rtti/RTTITypeName.hpp"

namespace test {
std::int32_t staticAnswer() {
  return 42;
}

std::int32_t staticSum(const std::int32_t lhs, const std::int32_t rhs) {
  return lhs + rhs;
}

void staticVoid() {}

struct UnregisteredArg {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::int32_t value;
};

UnregisteredArg staticReturnsUnregistered() {
  return UnregisteredArg{456};
}

void staticTakesUnregisteredArg(const UnregisteredArg) {}
}  // namespace test

REGISTER_TYPE_NAME(test::UnregisteredArg, "StaticFunctionTests.UnregisteredArg");

static_assert(requires { core::rtti::RTTIStaticTFunction("ok", &test::staticSum, "lhs", "rhs"); });
static_assert(!std::is_constructible_v<core::rtti::RTTIStaticTFunction<decltype(&test::staticSum)>, const core::IName&,
                                       decltype(&test::staticSum), const char*>);

TEST_CASE(
    "Given a static function with no return and no arguments, when invoked with a valid stack frame, then the "
    "call succeeds",
    "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticVoid", &test::staticVoid);
  auto frame = function.createStackFrame();

  REQUIRE_NOTHROW(function.invoke(frame));
}

TEST_CASE(
    "Given a static function with a return value and no arguments, when invoked with a valid return pointer, "
    "then the return value is written",
    "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticAnswer", &test::staticAnswer);
  auto frame = function.createStackFrame();
  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 42);
}

TEST_CASE(
    "Given a static function with two arguments, when invoked with valid argument pointers, then both "
    "arguments are forwarded in order",
    "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticSum", &test::staticSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 11;
  std::int32_t rhs = 9;
  frame.argPtr(0, &lhs);
  frame.argPtr(1, &rhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 20);
}

TEST_CASE(
    "Given a static function with argument names and return value, when arguments are queried, then names "
    "and types match the reflected signature",
    "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticSum_metadata", &test::staticSum, "lhs", "rhs");
  auto args = function.arguments();
  auto* intType = core::rtti::RTTITypeSystem::get().registry().getType(core::rtti::GetTypeName<std::int32_t>());

  REQUIRE(args.size() == 2);
  REQUIRE(std::string(args[0]->name()) == "lhs");
  REQUIRE(std::string(args[1]->name()) == "rhs");
  REQUIRE(args[0]->type() == intType);
  REQUIRE(args[1]->type() == intType);
}

TEST_CASE(
    "Given a static function with a return type, when invoke is called without a return pointer, then an "
    "exception is thrown",
    "[rtti][static_function][negative]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticAnswer_missing_return", &test::staticAnswer);
  auto frame = function.createStackFrame();

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a static function with arguments, when one argument pointer is missing, then invocation throws",
          "[rtti][static_function][negative]") {
  core::rtti::RTTITypeSystem::get().initialize();

  auto function = core::rtti::RTTIStaticTFunction("staticSum_missing_arg", &test::staticSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 10;
  frame.argPtr(0, &lhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a static function with an unregistered return type, when reflected, then construction throws",
          "[rtti][static_function][negative]") {
  core::rtti::RTTITypeSystem::get().initialize();

  REQUIRE_THROWS_AS(core::rtti::RTTIStaticTFunction("staticReturnsUnregistered", &test::staticReturnsUnregistered),
                    std::runtime_error);
}

TEST_CASE("Given a static function with an unregistered argument type, when reflected, then construction throws",
          "[rtti][static_function][negative]") {
  core::rtti::RTTITypeSystem::get().initialize();

  REQUIRE_THROWS_AS(
      core::rtti::RTTIStaticTFunction("staticTakesUnregistered", &test::staticTakesUnregisteredArg, "arg"),
      std::runtime_error);
}

TEST_CASE("Given a static function, when the function name is queried, then it returns the correct name",
          "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  const auto function = core::rtti::RTTIStaticTFunction("test_static_name", &test::staticAnswer);

  REQUIRE(std::string(function.name()) == "test_static_name");
}

TEST_CASE("Given a static function, when return type is queried, then it corresponds to the actual return type",
          "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  const auto function = core::rtti::RTTIStaticTFunction("staticAnswer_returntype", &test::staticAnswer);
  auto* intType = core::rtti::RTTITypeSystem::get().registry().getType(core::rtti::GetTypeName<std::int32_t>());

  REQUIRE(function.returnType() == intType);
}

TEST_CASE("Given a static function, when flags are queried, then isMember is false and isNative is true",
          "[rtti][static_function]") {
  core::rtti::RTTITypeSystem::get().initialize();

  const auto function = core::rtti::RTTIStaticTFunction("staticAnswer_flags", &test::staticAnswer);
  auto [isNative, isMember] = function.flags();

  REQUIRE(!isMember);
  REQUIRE(isNative);
}
