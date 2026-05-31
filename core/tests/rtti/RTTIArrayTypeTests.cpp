#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <type_traits>

#include "CString.hpp"
#include "Vector.hpp"
#include "rtti/RTTIArrayType.hpp"
#include "rtti/RTTIClassType.hpp"

using core::Vector;

/**
 * @brief A lightweight class element used for array descriptor tests.
 */
struct TrivialElement {
  // ReSharper disable once CppDeclaratorNeverUsed
  int value{0};

  bool operator==(const TrivialElement&) const = default;
};

/**
 * @brief A non-trivial class element used for string lifecycle coverage in array tests.
 */
struct NonTrivialElement {
  // ReSharper disable once CppDeclaratorNeverUsed
  std::string value;

  bool operator==(const NonTrivialElement&) const = default;
};

REGISTER_TYPE_NAME(TrivialElement, "trivial_element");
REGISTER_TYPE_NAME(NonTrivialElement, "non_trivial_element");

namespace {
using core::IName;
using core::rtti::RTTIArrayTType;
using core::rtti::RTTIArrayType;
using core::rtti::RTTIClassTType;
using core::rtti::RTTIType;
using core::rtti::RTTITypeKind;

/**
 * @brief Returns a first sample value for the test element type.
 *
 * @tparam T The element type under test.
 * @return A value suitable for positive-path array operations.
 */
template <typename T>
T valueA() {
  if constexpr (std::is_same_v<T, TrivialElement>) {
    return TrivialElement{11};
  } else {
    return NonTrivialElement{"alpha"};
  }
}

/**
 * @brief Returns a second sample value for the test element type.
 *
 * @tparam T The element type under test.
 * @return A value that differs from @code valueA<T>()@endcode.
 */
template <typename T>
T valueB() {
  if constexpr (std::is_same_v<T, TrivialElement>) {
    return TrivialElement{22};
  } else {
    return NonTrivialElement{"beta"};
  }
}

/**
 * @brief Returns a third sample value for the test element type.
 *
 * @tparam T The element type under test.
 * @return A value that differs from @code valueA<T>()@endcode and @code valueB<T>()@endcode.
 */
template <typename T>
T valueC() {
  if constexpr (std::is_same_v<T, TrivialElement>) {
    return TrivialElement{33};
  } else {
    return NonTrivialElement{"gamma"};
  }
}

/**
 * @brief Returns the canonical array descriptor name for the test element type.
 *
 * @tparam T The element type under test.
 * @return The expected array descriptor name text.
 */
template <typename T>
const char* arrayTypeName() {
  if constexpr (std::is_same_v<T, TrivialElement>) {
    return "array:trivial_element";
  } else {
    return "array:non_trivial_element";
  }
}
}  // namespace

TEMPLATE_TEST_CASE(
    "Given an RTTIArrayTType descriptor, when observed through RTTIArrayType and RTTIType, then metadata and inner "
    "type are "
    "correct",
    "[rtti][array_type][metadata]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  RTTIArrayType* asArray = &descriptor;
  RTTIType* asType = &descriptor;

  REQUIRE(asArray != nullptr);
  REQUIRE(asType != nullptr);
  REQUIRE(asType->kind() == RTTITypeKind::ARRAY);
  REQUIRE(asType->size() == sizeof(Vector<TestType>));
  REQUIRE(asType->alignment() == alignof(Vector<TestType>));
  REQUIRE(asArray->inner() == static_cast<const RTTIType*>(&inner));
  REQUIRE(asType->name() == IName(arrayTypeName<TestType>()));
  REQUIRE(asType->name().toString() == arrayTypeName<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when length capacity and maxLength are queried, then values are returned for null and "
    "live vectors",
    "[rtti][array_type][queries]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values{valueA<TestType>(), valueB<TestType>()};

  REQUIRE(descriptor.length(nullptr) == 0);
  REQUIRE(descriptor.capacity(nullptr) == 0);
  REQUIRE(descriptor.maxLength(nullptr) == 0);

  REQUIRE(descriptor.length(&values) == 2);
  REQUIRE(descriptor.capacity(&values) >= descriptor.length(&values));
  REQUIRE(descriptor.maxLength(&values) >= descriptor.length(&values));
}

TEMPLATE_TEST_CASE(
    "Given a non-empty array descriptor, when at front and back are queried, then mutable and const access return "
    "expected elements",
    "[rtti][array_type][element_access]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values{valueA<TestType>(), valueB<TestType>(), valueC<TestType>()};
  const auto& constValues = values;

  auto* atOne = static_cast<TestType*>(descriptor.at(&values, 1));
  const auto* constAtTwo = static_cast<const TestType*>(descriptor.at(&constValues, 2));
  auto* front = static_cast<TestType*>(descriptor.front(&values));
  const auto* constBack = static_cast<const TestType*>(descriptor.back(&constValues));

  REQUIRE(atOne != nullptr);
  REQUIRE(constAtTwo != nullptr);
  REQUIRE(front != nullptr);
  REQUIRE(constBack != nullptr);

  REQUIRE(*atOne == valueB<TestType>());
  REQUIRE(*constAtTwo == valueC<TestType>());
  REQUIRE(*front == valueA<TestType>());
  REQUIRE(*constBack == valueC<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when null pointers are passed to element accessors, then nullptr is returned",
    "[rtti][array_type][element_access][negative]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  REQUIRE(descriptor.at(nullptr, 0) == nullptr);
  REQUIRE(descriptor.front(nullptr) == nullptr);
  REQUIRE(descriptor.back(nullptr) == nullptr);
}

TEMPLATE_TEST_CASE("Given an array descriptor, when at is called with an out-of-range index, then nullptr is returned",
                   "[rtti][array_type][element_access][negative][bounds]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values{valueA<TestType>()};

  REQUIRE(descriptor.at(&values, 1) == nullptr);
  REQUIRE(static_cast<const RTTIArrayType&>(descriptor).at(&values, 5) == nullptr);
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor and a populated vector, when begin and end are used, then the full element range is "
    "traversable",
    "[rtti][array_type][iterators]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values{valueA<TestType>(), valueB<TestType>(), valueC<TestType>()};

  auto begin = descriptor.begin(&values);
  auto end = descriptor.end(&values);

  REQUIRE((end - begin) == 3);
  REQUIRE(*static_cast<TestType*>(begin.get()) == valueA<TestType>());

  ++begin;
  REQUIRE(*static_cast<TestType*>(begin.get()) == valueB<TestType>());

  auto rbegin = descriptor.rbegin(&values);
  auto rend = descriptor.rend(&values);

  REQUIRE((rbegin - rend) == -3);
  REQUIRE(*static_cast<TestType*>(rbegin.get()) == valueC<TestType>());
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when begin and end are requested with a null array pointer, then both iterators are "
    "equal",
    "[rtti][array_type][iterators][negative]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  REQUIRE(descriptor.begin(nullptr) == descriptor.end(nullptr));
  REQUIRE(descriptor.rbegin(nullptr) == descriptor.rend(nullptr));
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when pushBack insert erase remove replace and popBack are called, then the vector "
    "contents update correctly",
    "[rtti][array_type][modifiers]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values;

  auto a = valueA<TestType>();
  auto b = valueB<TestType>();
  auto c = valueC<TestType>();

  descriptor.pushBack(&values, &a);
  descriptor.pushBack(&values, &b);
  REQUIRE(values.size() == 2);
  REQUIRE(values[0] == a);
  REQUIRE(values[1] == b);

  descriptor.insert(&values, 1, &c);
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == a);
  REQUIRE(values[1] == c);
  REQUIRE(values[2] == b);

  descriptor.replace(&values, 1, &b);
  REQUIRE(values[1] == b);

  descriptor.erase(&values, 0);
  REQUIRE(values.size() == 2);

  descriptor.remove(&values, 1);
  REQUIRE(values.size() == 1);

  descriptor.popBack(&values);
  REQUIRE(values.empty());
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when null arrays or null values are passed to mutating operations, then the operations "
    "are safe no-ops",
    "[rtti][array_type][modifiers][negative]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values{valueA<TestType>(), valueB<TestType>()};
  const auto before = values;

  auto replacement = valueC<TestType>();

  descriptor.insert(nullptr, 0, &replacement);
  descriptor.pushBack(nullptr, &replacement);
  descriptor.popBack(nullptr);
  descriptor.erase(nullptr, 0);
  descriptor.remove(nullptr, 0);
  descriptor.replace(nullptr, 0, &replacement);
  descriptor.reserve(nullptr, 8);
  descriptor.resize(nullptr, 8);
  descriptor.shrinkToFit(nullptr);
  descriptor.clear(nullptr);

  descriptor.insert(&values, 1, nullptr);
  descriptor.pushBack(&values, nullptr);
  descriptor.replace(&values, 0, nullptr);

  REQUIRE(values == before);
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor, when reserve resize shrinkToFit and clear are used, then capacity and length evolve "
    "consistently",
    "[rtti][array_type][capacity]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  Vector<TestType> values;

  descriptor.reserve(&values, 10);
  REQUIRE(values.capacity() >= 10);

  descriptor.resize(&values, 5);
  REQUIRE(values.size() == 5);

  values[0] = valueA<TestType>();
  values[1] = valueB<TestType>();
  values[2] = valueC<TestType>();

  descriptor.resize(&values, 2);
  REQUIRE(values.size() == 2);

  descriptor.shrinkToFit(&values);
  REQUIRE(values.capacity() >= values.size());

  descriptor.clear(&values);
  REQUIRE(values.empty());
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor viewed through RTTIType, when assign and equals are called, then array values compare "
    "and copy correctly including null combinations",
    "[rtti][array_type][rttitype][assign_equals]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  RTTIType& asType = descriptor;
  const RTTIType& constType = descriptor;

  Vector<TestType> source{valueA<TestType>(), valueB<TestType>()};
  Vector<TestType> destination{valueC<TestType>()};

  asType.assign(&destination, &source);
  REQUIRE(destination == source);
  REQUIRE(constType.equals(&destination, &source));

  asType.assign(nullptr, &source);
  REQUIRE(destination == source);

  Vector<TestType> different{valueC<TestType>()};
  REQUIRE_FALSE(constType.equals(&destination, &different));
  REQUIRE(constType.equals(nullptr, nullptr));
  REQUIRE_FALSE(constType.equals(&destination, nullptr));
  REQUIRE_FALSE(constType.equals(nullptr, &destination));
}

TEMPLATE_TEST_CASE(
    "Given an array descriptor viewed through RTTIType, when allocate construct destruct create and destroy are "
    "called, then full lifetime operations succeed",
    "[rtti][array_type][rttitype][lifecycle]", TrivialElement, NonTrivialElement) {
  RTTIClassTType<TestType> inner;
  RTTIArrayTType<TestType> descriptor(&inner);

  RTTIType& asType = descriptor;

  void* allocated = asType.allocate();
  REQUIRE(allocated != nullptr);
  REQUIRE(reinterpret_cast<std::uintptr_t>(allocated) % alignof(Vector<TestType>) == 0);

  asType.construct(allocated);
  auto* allocatedVector = static_cast<Vector<TestType>*>(allocated);
  REQUIRE(allocatedVector->empty());

  allocatedVector->pushBack(valueA<TestType>());
  REQUIRE(allocatedVector->size() == 1);

  asType.destruct(allocated);
  asType.deallocate(allocated);

  void* created = asType.create();
  REQUIRE(created != nullptr);
  auto* createdVector = static_cast<Vector<TestType>*>(created);
  REQUIRE(createdVector->empty());

  createdVector->pushBack(valueB<TestType>());
  REQUIRE(createdVector->size() == 1);

  asType.destroy(created);
  asType.destroy(nullptr);
}
