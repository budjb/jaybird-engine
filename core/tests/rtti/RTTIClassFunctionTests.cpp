#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "rtti/RTTIClassFunction.hpp"
#include "rtti/RTTIName.hpp"

namespace test {
struct UnregisteredArg {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::int32_t value;
};

struct UnregisteredReturn {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::int32_t value;
};

class Sample {
 public:
  // ReSharper disable once CppMemberFunctionMayBeStatic
  void touch() const {}

  // ReSharper disable once CppMemberFunctionMayBeStatic
  [[nodiscard]] std::int32_t answer() const {
    return 42;
  }

  // ReSharper disable once CppMemberFunctionMayBeStatic
  [[nodiscard]] std::int32_t sum(const std::int32_t lhs, const std::int32_t rhs) const {
    return lhs + rhs;
  }

  // ReSharper disable once CppMemberFunctionMayBeStatic
  void takesUnregisteredArg(const UnregisteredArg) const {}

  // ReSharper disable once CppMemberFunctionMayBeStatic
  [[nodiscard]] UnregisteredReturn returnsUnregisteredType() const {
    return UnregisteredReturn{123};
  }

  static void staticTouch() {}

  [[nodiscard]] static std::int32_t staticAnswer() {
    return 7;
  }

  [[nodiscard]] static std::int32_t staticSum(const std::int32_t lhs, const std::int32_t rhs) {
    return lhs + rhs;
  }

  static void staticTakesUnregisteredArg(const UnregisteredArg) {}

  [[nodiscard]] static UnregisteredReturn staticReturnsUnregisteredType() {
    return UnregisteredReturn{123};
  }
};
}  // namespace test

REGISTER_TYPE_NAME(test::Sample, "ClassFunctionTests.Sample");
REGISTER_TYPE_NAME(test::UnregisteredArg, "ClassFunctionTests.UnregisteredArg");
REGISTER_TYPE_NAME(test::UnregisteredReturn, "ClassFunctionTests.UnregisteredReturn");

static_assert(requires { core::rtti::RTTIClassTFunction("ok", &test::Sample::sum, "lhs", "rhs"); });
static_assert(requires { core::rtti::RTTIClassTFunction("ok_static", &test::Sample::staticSum, "lhs", "rhs"); });
static_assert(!std::is_constructible_v<core::rtti::RTTIClassTFunction<decltype(&test::Sample::sum)>, const core::Name&,
                                       decltype(&test::Sample::sum), const char*>);

TEST_CASE(
    "Given a member function with no return and no arguments, when invoked with a valid \"this\" pointer, then the "
    "call "
    "succeeds",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("touch", &test::Sample::touch);
  auto frame = function.createStackFrame();
  test::Sample instance;
  frame.thisPtr(&instance);

  REQUIRE_NOTHROW(function.invoke(frame));
}

TEST_CASE(
    "Given a member function with a return value, when invoked with valid this and return pointers, then the return "
    "value is written",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("answer", &test::Sample::answer);
  auto frame = function.createStackFrame();
  test::Sample instance;
  frame.thisPtr(&instance);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 42);
}

TEST_CASE(
    "Given a member function with two arguments, when invoked with valid argument pointers, then both arguments are "
    "forwarded in order",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("sum", &test::Sample::sum, "lhs", "rhs");
  auto frame = function.createStackFrame();
  test::Sample instance;
  frame.thisPtr(&instance);

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
    "Given argument names during function registration, when arguments are queried, then names and types match the "
    "reflected signature",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("sum_metadata", &test::Sample::sum, "lhs", "rhs");
  auto args = function.arguments();
  auto* intType = core::rtti::RTTISystem::get().registry().getType(core::rtti::GetRTTIName<std::int32_t>());

  REQUIRE(args.size() == 2);
  REQUIRE(std::string(args[0]->name()) == "lhs");
  REQUIRE(std::string(args[1]->name()) == "rhs");
  REQUIRE(args[0]->type() == intType);
  REQUIRE(args[1]->type() == intType);
}

TEST_CASE(
    "Given a member function invocation without a \"this\" pointer, when invoke is called, then an exception is thrown",
    "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("answer_missing_this", &test::Sample::answer);
  auto frame = function.createStackFrame();

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE(
    "Given a member function with a non-void return type, when invoke is called without a return pointer, then an "
    "exception is thrown",
    "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("answer_missing_return", &test::Sample::answer);
  auto frame = function.createStackFrame();
  test::Sample instance;
  frame.thisPtr(&instance);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a member function with arguments, when one argument pointer is missing, then invocation throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("sum_missing_arg", &test::Sample::sum, "lhs", "rhs");
  auto frame = function.createStackFrame();
  test::Sample instance;
  frame.thisPtr(&instance);

  std::int32_t lhs = 10;
  frame.argPtr(0, &lhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a member function with an unregistered argument type, when reflected, then construction throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(core::rtti::RTTIClassTFunction("bad_arg", &test::Sample::takesUnregisteredArg, "value"),
                    std::runtime_error);
}

TEST_CASE("Given a member function with an unregistered return type, when reflected, then construction throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(core::rtti::RTTIClassTFunction("bad_return", &test::Sample::returnsUnregisteredType),
                    std::runtime_error);
}

TEST_CASE("Given a member function, when the function name is queried, then it returns the correct name",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("member_test_name", &test::Sample::answer);

  REQUIRE(std::string(function.name()) == "member_test_name");
}

TEST_CASE("Given a member function, when return type is queried, then it corresponds to the actual return type",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("answer_returntype", &test::Sample::answer);
  auto* intType = core::rtti::RTTISystem::get().registry().getType(core::rtti::GetRTTIName<std::int32_t>());

  REQUIRE(function.returnType() == intType);
}

TEST_CASE("Given a member function with no return type, when return type is queried, then nullptr is returned",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("touch_no_return", &test::Sample::touch);

  REQUIRE(function.returnType() == nullptr);
}

TEST_CASE("Given a member function, when flags are queried, then isMember is true and isNative is true",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("answer_flags", &test::Sample::answer);
  auto [isNative, isStatic] = function.flags();

  REQUIRE(!isStatic);
  REQUIRE(isNative);
}

TEST_CASE("Given a member function, when a stack frame is created, then the frame includes a \"this\" pointer slot",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("sum_frame", &test::Sample::sum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  // Verify that thisPtr() returns nullptr when not set
  REQUIRE(frame.thisPtr<void>() == nullptr);

  // Set a "this" pointer and verify it's preserved
  test::Sample instance;
  frame.thisPtr(&instance);
  REQUIRE(frame.thisPtr<test::Sample>() == &instance);
}

TEST_CASE(
    "Given a member function with no arguments, when a stack frame is created, then only this and return slots "
    "exist",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("answer_noargs", &test::Sample::answer);
  const auto frame = function.createStackFrame();

  // Should have "this" pointer and return value, but no argument slots
  REQUIRE(frame.argPtr<void>(0) == nullptr);
  REQUIRE(frame.argPtr<void>(1) == nullptr);
}

TEST_CASE("Given a member function with a void return, when a stack frame is created, then no return slot exists",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("touch_noreturn", &test::Sample::touch);
  auto frame = function.createStackFrame();

  // Should have no return slot
  REQUIRE(frame.returnPtr<void>() == nullptr);
}

TEST_CASE(
    "Given a static class function with no return and no arguments, when invoked with a default frame, then the call "
    "succeeds",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_touch", &test::Sample::staticTouch);
  auto frame = function.createStackFrame();

  REQUIRE_NOTHROW(function.invoke(frame));
}

TEST_CASE(
    "Given a static class function with a return value, when invoked with a valid return pointer, then the return "
    "value is written",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_answer", &test::Sample::staticAnswer);
  auto frame = function.createStackFrame();
  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_NOTHROW(function.invoke(frame));
  REQUIRE(result == 7);
}

TEST_CASE(
    "Given a static class function with two arguments, when invoked with valid argument pointers, then both "
    "arguments are forwarded in order",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_sum", &test::Sample::staticSum, "lhs", "rhs");
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
    "Given argument names for a static class function, when arguments are queried, then names and types match the "
    "reflected signature",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_sum_metadata", &test::Sample::staticSum, "lhs", "rhs");
  auto args = function.arguments();
  auto* intType = core::rtti::RTTISystem::get().registry().getType(core::rtti::GetRTTIName<std::int32_t>());

  REQUIRE(args.size() == 2);
  REQUIRE(std::string(args[0]->name()) == "lhs");
  REQUIRE(std::string(args[1]->name()) == "rhs");
  REQUIRE(args[0]->type() == intType);
  REQUIRE(args[1]->type() == intType);
}

TEST_CASE(
    "Given a static class function with a non-void return type, when invoke is called without a return pointer, then "
    "an exception is thrown",
    "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_answer_missing_return", &test::Sample::staticAnswer);
  auto frame = function.createStackFrame();

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a static class function with arguments, when one argument pointer is missing, then invocation throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  auto function = core::rtti::RTTIClassTFunction("static_sum_missing_arg", &test::Sample::staticSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  std::int32_t lhs = 10;
  frame.argPtr(0, &lhs);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE_THROWS_AS(function.invoke(frame), std::runtime_error);
}

TEST_CASE("Given a static class function with an unregistered argument type, when reflected, then construction throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(
      core::rtti::RTTIClassTFunction("static_bad_arg", &test::Sample::staticTakesUnregisteredArg, "value"),
      std::runtime_error);
}

TEST_CASE("Given a static class function with an unregistered return type, when reflected, then construction throws",
          "[rtti][class_function][negative]") {
  core::rtti::RTTISystem::get().initialize();

  REQUIRE_THROWS_AS(core::rtti::RTTIClassTFunction("static_bad_return", &test::Sample::staticReturnsUnregisteredType),
                    std::runtime_error);
}

TEST_CASE("Given a static class function, when flags are queried, then isStatic is true and isNative is true",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("static_answer_flags", &test::Sample::staticAnswer);
  auto [isNative, isStatic] = function.flags();

  REQUIRE(isStatic);
  REQUIRE(isNative);
}

TEST_CASE(
    "Given a static class function, when a stack frame is created, then the frame does not include a this pointer "
    "slot",
    "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("static_sum_frame", &test::Sample::staticSum, "lhs", "rhs");
  auto frame = function.createStackFrame();

  REQUIRE(frame.thisPtr<void>() == nullptr);

  test::Sample instance;
  frame.thisPtr(&instance);
  REQUIRE(frame.thisPtr<test::Sample>() == nullptr);
}

TEST_CASE("Given a static class function with a void return, when a stack frame is created, then no return slot exists",
          "[rtti][class_function]") {
  core::rtti::RTTISystem::get().initialize();

  const auto function = core::rtti::RTTIClassTFunction("static_touch_noreturn", &test::Sample::staticTouch);
  const auto frame = function.createStackFrame();

  REQUIRE(frame.returnPtr<void>() == nullptr);
}
