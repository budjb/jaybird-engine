#include <atomic>
#include <catch2/catch_test_macros.hpp>

#include "CString.hpp"
#include "Hash.hpp"
#include "rtti/TypeKind.hpp"
#include "rtti/TypeRegistry.hpp"
#include "rtti/registration/Macros.hpp"
#include "rtti/registration/Specialization.hpp"
#include "rtti/registration/TypeDefinition.hpp"
#include "rtti/registration/TypeName.hpp"
#include "rtti/registration/TypeRegistrar.hpp"

namespace rtti_registration_tests {
struct UsesNameMember {
  static constexpr char NAME[] = "uses_name_member";
};

struct UsesAutoNameMember {
  static constexpr auto NAME = "uses_auto_name_member";
};

struct UsesTypeMap {};

class AutoRegisteredClass {
 public:
  static constexpr auto NAME = "auto_registered_class_test";
};
}  // namespace rtti_registration_tests

template <>
struct core::rtti::TypeName<rtti_registration_tests::UsesTypeMap> {
  static constexpr CString value{"uses_type_map"};
};

inline std::atomic g_classDefineCalls{0};
inline core::rtti::IClassType* g_lastDefinedClass = nullptr;

RTTI_REGISTER_TYPE(short, "short_auto_registered_test");
RTTI_REGISTER_CLASS(rtti_registration_tests::AutoRegisteredClass, {
  ++::g_classDefineCalls;
  ::g_lastDefinedClass = type;
});

namespace {
using core::IName;
using core::rtti::HasTypeNameMapping;
using core::rtti::HasTypeNameMember;
using core::rtti::IFundamentalDefinition;
using core::rtti::Specialization;
using core::rtti::TypeKind;
using core::rtti::TypeRegistrar;
using core::rtti::TypeRegistry;

inline std::atomic g_customDeclared{false};
inline std::atomic g_customDefineObservedDeclare{false};
inline std::atomic g_declareOnlyCalled{false};

void customDeclare(TypeRegistry* registry) {
  (void)registry;
  g_customDeclared.store(true, std::memory_order_relaxed);
}

void customDefine() {
  g_customDefineObservedDeclare.store(g_customDeclared.load(std::memory_order_relaxed), std::memory_order_relaxed);
}

void declareOnly(TypeRegistry* registry) {
  (void)registry;
  g_declareOnlyCalled.store(true, std::memory_order_relaxed);
}

template <typename T>
concept HasFundamentalDefinition = requires { typename IFundamentalDefinition<T>; };

static_assert(HasTypeNameMember<rtti_registration_tests::UsesNameMember>);
static_assert(HasTypeNameMember<rtti_registration_tests::UsesAutoNameMember>);
static_assert(!HasTypeNameMapping<rtti_registration_tests::UsesNameMember>);
static_assert(!HasTypeNameMember<rtti_registration_tests::UsesTypeMap>);
static_assert(HasTypeNameMapping<rtti_registration_tests::UsesTypeMap>);
static_assert(!HasFundamentalDefinition<void>);
static_assert(!HasFundamentalDefinition<std::nullptr_t>);
static_assert(HasFundamentalDefinition<int>);

}  // namespace

TEST_CASE("Given FixedString and TypeName mapping, when queried, then compile-time mapping resolves to expected text",
          "[rtti][registration][type_name]") {
  REQUIRE(core::rtti::GetTypeName<rtti_registration_tests::UsesTypeMap>() == "uses_type_map");
}

TEST_CASE("Given a type name source, when Specialization::of is used, then hash identity matches FNV-1a",
          "[rtti][registration][specialization]") {
  constexpr auto fromMember = Specialization::of<rtti_registration_tests::UsesNameMember>();
  constexpr auto fromAutoMember = Specialization::of<rtti_registration_tests::UsesAutoNameMember>();
  constexpr auto fromMapping = Specialization::of<rtti_registration_tests::UsesTypeMap>();

  REQUIRE(fromMember.m_hash == core::fnv1a_64("uses_name_member"));
  REQUIRE(fromAutoMember.m_hash == core::fnv1a_64("uses_auto_name_member"));
  REQUIRE(fromMapping.m_hash == core::fnv1a_64("uses_type_map"));
}

TEST_CASE(
    "Given custom TypeRegistrar entries, when registerTypes is called, then declaration and definition phases run in "
    "order",
    "[rtti][registration][type_registrar]") {
  static TypeRegistrar withDefine{&customDeclare, &customDefine};
  static TypeRegistrar onlyDeclare{&declareOnly};
  (void)withDefine;
  (void)onlyDeclare;

  g_customDeclared.store(false, std::memory_order_relaxed);
  g_customDefineObservedDeclare.store(false, std::memory_order_relaxed);
  g_declareOnlyCalled.store(false, std::memory_order_relaxed);

  TypeRegistrar::registerTypes();

  REQUIRE(g_customDeclared.load(std::memory_order_relaxed));
  REQUIRE(g_customDefineObservedDeclare.load(std::memory_order_relaxed));
  REQUIRE(g_declareOnlyCalled.load(std::memory_order_relaxed));
}

TEST_CASE(
    "Given auto-registered fundamental and class macros, when registerTypes is called, then both are discoverable",
    "[rtti][registration][auto_registration]") {
  const int beforeDefineCalls = g_classDefineCalls.load(std::memory_order_relaxed);
  TypeRegistrar::registerTypes();

  TypeRegistry* registry = TypeRegistry::get();

  const IName shortName("short_auto_registered_test");
  REQUIRE(registry->hasType(shortName));
  REQUIRE(registry->getType(shortName) != nullptr);
  REQUIRE(registry->getType(shortName)->kind() == TypeKind::FUNDAMENTAL);

  const IName className("auto_registered_class_test");
  REQUIRE(registry->hasType(className));
  REQUIRE(registry->getType(className) != nullptr);
  REQUIRE(registry->getType(className)->kind() == TypeKind::CLASS);
  REQUIRE(registry->getClass(className) != nullptr);

  REQUIRE(g_classDefineCalls.load(std::memory_order_relaxed) > beforeDefineCalls);
  REQUIRE(g_lastDefinedClass == registry->getClass(className));
}
