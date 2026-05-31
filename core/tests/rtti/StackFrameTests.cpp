#include <catch2/catch_test_macros.hpp>
#include <cstdint>

#include "rtti/StackFrame.hpp"

namespace test {
struct Sample {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::int32_t value;
};
}  // namespace test

TEST_CASE(
    "Given a stack frame for a member function with arguments and return, when all pointers are stored and "
    "retrieved, then each slot preserves the exact pointer value",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(2, true, true);

  test::Sample instance{42};
  std::int32_t lhs = 3;
  std::int32_t rhs = 4;
  std::int32_t result = 0;

  frame.thisPtr(&instance);
  frame.argPtr(0, &lhs);
  frame.argPtr(1, &rhs);
  frame.returnPtr(&result);

  REQUIRE(frame.thisPtr<test::Sample>() == &instance);
  REQUIRE(frame.argPtr<std::int32_t>(0) == &lhs);
  REQUIRE(frame.argPtr<std::int32_t>(1) == &rhs);
  REQUIRE(frame.returnPtr<std::int32_t>() == &result);
}

TEST_CASE("Given a stack frame without a this pointer slot, when thisPtr is queried, then nullptr is returned",
          "[rtti][stack_frame]") {
  const core::rtti::StackFrame frame(2, true, false);

  REQUIRE(frame.thisPtr<std::int32_t>() == nullptr);
}

TEST_CASE(
    "Given a stack frame with a this pointer slot, when thisPtr is set without a value, then the write is "
    "silently ignored",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(0, false, true);

  frame.thisPtr(static_cast<std::int32_t*>(nullptr));

  REQUIRE(frame.thisPtr<std::int32_t>() == nullptr);
}

TEST_CASE("Given a stack frame, when invalid argument indices are addressed, then reads return nullptr",
          "[rtti][stack_frame][negative]") {
  core::rtti::StackFrame frame(1, false, false);

  REQUIRE(frame.argPtr<std::int32_t>(1) == nullptr);
}

TEST_CASE(
    "Given a stack frame without an argument slot, when argPtr is set for an invalid index, then the write "
    "is ignored",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(1, false, false);

  std::int32_t value = 1;
  frame.argPtr(1, &value);

  REQUIRE(frame.argPtr<std::int32_t>(1) == nullptr);
}

TEST_CASE(
    "Given a stack frame without an argument slot, when a valid argument is set and retrieved, then the "
    "pointer is preserved",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(2, false, false);

  std::int32_t value0 = 1;
  std::int32_t value1 = 2;
  frame.argPtr(0, &value0);
  frame.argPtr(1, &value1);

  REQUIRE(frame.argPtr<std::int32_t>(0) == &value0);
  REQUIRE(frame.argPtr<std::int32_t>(1) == &value1);
}

TEST_CASE("Given a stack frame without a return slot, when returnPtr is queried, then nullptr is returned",
          "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(2, false, false);

  REQUIRE(frame.returnPtr<std::int32_t>() == nullptr);
}

TEST_CASE(
    "Given a stack frame with a return slot, when returnPtr is set without a value, then the write is "
    "silently ignored",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(0, true, false);

  frame.returnPtr(static_cast<std::int32_t*>(nullptr));

  REQUIRE(frame.returnPtr<std::int32_t>() == nullptr);
}

TEST_CASE(
    "Given a stack frame with a return slot, when a return value pointer is set and retrieved, then the "
    "pointer is preserved",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(0, true, false);

  std::int32_t result = 0;
  frame.returnPtr(&result);

  REQUIRE(frame.returnPtr<std::int32_t>() == &result);
}

TEST_CASE(
    "Given a stack frame with a member function signature, when the layout is correct, then all slots are at "
    "the expected offsets",
    "[rtti][stack_frame]") {
  // Frame: [this][arg0][arg1][return]
  core::rtti::StackFrame frame(2, true, true);

  test::Sample obj{123};
  std::int32_t arg0 = 1;
  std::int32_t arg1 = 2;
  std::int32_t ret = 0;

  frame.thisPtr(&obj);
  frame.argPtr(0, &arg0);
  frame.argPtr(1, &arg1);
  frame.returnPtr(&ret);

  // Verify all values are at the right place
  REQUIRE(frame.thisPtr<test::Sample>() == &obj);
  REQUIRE(frame.argPtr<std::int32_t>(0) == &arg0);
  REQUIRE(frame.argPtr<std::int32_t>(1) == &arg1);
  REQUIRE(frame.returnPtr<std::int32_t>() == &ret);
}

TEST_CASE(
    "Given a stack frame for a free function, when the layout is correct, then all slots are at the expected "
    "offsets",
    "[rtti][stack_frame]") {
  // Frame: [arg0][arg1][return] (no this)
  core::rtti::StackFrame frame(2, true, false);

  std::int32_t arg0 = 10;
  std::int32_t arg1 = 20;
  std::int32_t ret = 0;

  frame.argPtr(0, &arg0);
  frame.argPtr(1, &arg1);
  frame.returnPtr(&ret);

  // Verify all values are at the right place
  REQUIRE(frame.argPtr<std::int32_t>(0) == &arg0);
  REQUIRE(frame.argPtr<std::int32_t>(1) == &arg1);
  REQUIRE(frame.returnPtr<std::int32_t>() == &ret);
  REQUIRE(frame.thisPtr<void>() == nullptr);
}

TEST_CASE("Given a stack frame, when setting a pointer with nullptr value, then the write is silently ignored",
          "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(1, true, false);

  std::int32_t value = 42;
  frame.argPtr(0, &value);
  REQUIRE(frame.argPtr<std::int32_t>(0) == &value);

  // Try to set nullptr — should be ignored
  frame.argPtr(0, static_cast<std::int32_t*>(nullptr));
  REQUIRE(frame.argPtr<std::int32_t>(0) == &value);
}

TEST_CASE(
    "Given a stack frame with many arguments, when all arguments are set and retrieved, then each pointer is "
    "preserved in order",
    "[rtti][stack_frame]") {
  core::rtti::StackFrame frame(5, true, false);

  std::int32_t values[5] = {1, 2, 3, 4, 5};

  for (int i = 0; i < 5; ++i) {
    frame.argPtr(i, &values[i]);
  }

  std::int32_t result = 0;
  frame.returnPtr(&result);

  for (int i = 0; i < 5; ++i) {
    REQUIRE(frame.argPtr<std::int32_t>(i) == &values[i]);
  }
  REQUIRE(frame.returnPtr<std::int32_t>() == &result);
}
