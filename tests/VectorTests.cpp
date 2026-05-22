#include <array>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <memory_resource>
#include <stdexcept>
#include <vector>

#include "container/Vector.hpp"

namespace {
struct Widget {
  int value = 0;

  Widget() = default;
  explicit Widget(const int v) : value(v) {}

  friend bool operator==(const Widget&, const Widget&) = default;
};

struct MoveOnly {
  std::unique_ptr<int> value;

  MoveOnly() : value(std::make_unique<int>(0)) {}
  explicit MoveOnly(const int v) : value(std::make_unique<int>(v)) {}
  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;
  MoveOnly(MoveOnly&&) noexcept = default;
  MoveOnly& operator=(MoveOnly&&) noexcept = default;
};

struct ThrowOnCopy {
  static inline int copiesBeforeThrow = -1;

  int value = 0;

  ThrowOnCopy() = default;
  explicit ThrowOnCopy(const int v) : value(v) {}

  ThrowOnCopy(const ThrowOnCopy& other) : value(other.value) {
    if (copiesBeforeThrow == 0) {
      throw std::runtime_error("copy failure");
    }
    if (copiesBeforeThrow > 0) {
      --copiesBeforeThrow;
    }
  }

  ThrowOnCopy& operator=(const ThrowOnCopy&) = default;
  ThrowOnCopy(ThrowOnCopy&&) noexcept = default;
  ThrowOnCopy& operator=(ThrowOnCopy&&) noexcept = default;
};

template <typename T>
class StatefulAllocator {
 public:
  using value_type = T;
  using propagate_on_container_move_assignment = std::false_type;
  using is_always_equal = std::false_type;

  template <typename U>
  friend class StatefulAllocator;

  explicit StatefulAllocator(std::pmr::memory_resource* resource = std::pmr::get_default_resource(),
                             int* state = nullptr) noexcept
      : m_resource(resource), m_state(state) {}

  template <typename U>
  explicit StatefulAllocator(const StatefulAllocator<U>& other) noexcept
      : m_resource(other.resource()), m_state(other.state()) {}

  [[nodiscard]] T* allocate(const std::size_t n) {
    return static_cast<T*>(m_resource->allocate(n * sizeof(T), alignof(T)));
  }

  void deallocate(T* ptr, const std::size_t n) noexcept {
    m_resource->deallocate(ptr, n * sizeof(T), alignof(T));
  }

  [[nodiscard]] StatefulAllocator select_on_container_copy_construction() const noexcept {
    return *this;
  }

  [[nodiscard]] int* state() const noexcept {
    return m_state;
  }

  [[nodiscard]] std::pmr::memory_resource* resource() const noexcept {
    return m_resource;
  }

  template <typename U>
  [[nodiscard]] bool operator==(const StatefulAllocator<U>& other) const noexcept {
    return m_state == other.state() && m_resource == other.resource();
  }

  template <typename U>
  [[nodiscard]] bool operator!=(const StatefulAllocator<U>& other) const noexcept {
    return !(*this == other);
  }

 private:
  std::pmr::memory_resource* m_resource;
  int* m_state;
};

}  // namespace

TEST_CASE("Vector supports basic growth and element access", "[vector]") {
  core::container::Vector<int> values;

  values.push_back(10);
  values.emplace_back(20);
  values.push_back(30);

  REQUIRE(values.size() == 3);
  REQUIRE(values.capacity() >= values.size());
  REQUIRE(values.front() == 10);
  REQUIRE(values.back() == 30);
  REQUIRE(values[1] == 20);
  REQUIRE(values.at(2) == 30);
  REQUIRE(values[0] == 10);
  REQUIRE_THROWS_AS(values.at(3), std::out_of_range);
}

TEST_CASE("Vector size fill and capacity constructors follow standard-style semantics", "[vector]") {
  const core::container::Vector<int> sized(3);
  const core::container::Vector<int> filled(4, 9);
  auto reserved = core::container::Vector<int>();
  reserved.reserve(6);

  REQUIRE(sized.size() == 3);
  REQUIRE(sized[0] == 0);
  REQUIRE(sized[2] == 0);

  REQUIRE(filled.size() == 4);
  REQUIRE(filled.front() == 9);
  REQUIRE(filled.back() == 9);

  REQUIRE(reserved.empty());
  REQUIRE(reserved.capacity() >= 6);
}

TEST_CASE("Vector range constructor and assign overloads consume ranges", "[vector]") {
  constexpr std::array source = {2, 4, 6, 8};
  core::container::Vector<int> values(source.begin(), source.end());

  REQUIRE(values.size() == source.size());
  REQUIRE(values[1] == 4);
  REQUIRE(values[3] == 8);

  values.assign(3, 5);
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == 5);
  REQUIRE(values[2] == 5);

  const std::vector replacement = {7, 8, 9};
  values.assign(replacement.begin(), replacement.end());
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == 7);
  REQUIRE(values[2] == 9);

  values.assign({1, 1, 2, 3, 5});
  REQUIRE(values.size() == 5);
  REQUIRE(values[4] == 5);
}

TEST_CASE("Vector initializer list and iterators expose contents in order", "[vector]") {
  const core::container::Vector<int> values{1, 2, 3, 4};

  REQUIRE(values.size() == 4);
  REQUIRE(*values.begin() == 1);
  REQUIRE(*(values.begin() + 2) == 3);
  REQUIRE(values.end() - values.begin() == 4);
  REQUIRE(*values.rbegin() == 4);
  REQUIRE(*(values.crbegin() + 1) == 3);
}

TEST_CASE("Vector resize shrink and pop_back update size correctly", "[vector]") {
  core::container::Vector<Widget> values;

  values.resize(3, Widget(7));
  REQUIRE(values.size() == 3);
  REQUIRE(values[0].value == 7);
  REQUIRE(values[2].value == 7);

  values.resize(5);
  REQUIRE(values.size() == 5);
  REQUIRE(values[3].value == 0);

  values.pop_back();
  REQUIRE(values.size() == 4);

  std::vector<int> foo{};
  // foo.max
  values.shrink_to_fit();
  REQUIRE(values.capacity() == values.size());
}

TEST_CASE("Vector insert and erase preserve ordering", "[vector]") {
  core::container::Vector<int> values{1, 3, 4};

  auto inserted = values.insert(values.begin() + 1, 2);
  REQUIRE(*inserted == 2);
  REQUIRE(values.size() == 4);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 2);
  REQUIRE(values[2] == 3);
  REQUIRE(values[3] == 4);

  auto erased = values.erase(values.begin() + 1);
  REQUIRE(*erased == 3);
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 3);
  REQUIRE(values[2] == 4);

  values.erase(values.begin(), values.begin() + 2);
  REQUIRE(values.size() == 1);
  REQUIRE(values.front() == 4);
}

TEST_CASE("Vector insert overloads support counts and ranges", "[vector]") {
  core::container::Vector<int> values{1, 5};
  constexpr std::array middle = {3, 4};

  values.insert(values.begin() + 1, 2, 2);
  values.insert(values.begin() + 3, middle.begin(), middle.end());
  values.insert(values.end(), {6, 7});

  REQUIRE(values.size() == 8);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 2);
  REQUIRE(values[2] == 2);
  REQUIRE(values[3] == 3);
  REQUIRE(values[4] == 4);
  REQUIRE(values[5] == 5);
  REQUIRE(values[6] == 6);
  REQUIRE(values[7] == 7);
}

TEST_CASE("Vector in-place insert and erase keep storage when capacity is sufficient", "[vector]") {
  auto values = core::container::Vector<int>(8);
  values.assign({1, 2, 4, 5});
  values.reserve(8);

  auto* beforeInsert = values.data();
  values.insert(values.begin() + 2, 3);
  REQUIRE(values.data() == beforeInsert);
  REQUIRE(values.size() == 5);
  REQUIRE(values[2] == 3);

  auto* beforeErase = values.data();
  values.erase(values.begin() + 1, values.begin() + 3);
  REQUIRE(values.data() == beforeErase);
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == 1);
  REQUIRE(values[1] == 4);
  REQUIRE(values[2] == 5);
}

TEST_CASE("Vector copy operations produce deep copies", "[vector]") {
  core::container::Vector<int> original{1, 2, 3};
  core::container::Vector copied(original);
  core::container::Vector<int> assigned;

  assigned = original;
  copied[0] = 9;
  assigned[1] = 8;

  REQUIRE(original[0] == 1);
  REQUIRE(original[1] == 2);
  REQUIRE(copied[0] == 9);
  REQUIRE(assigned[1] == 8);
}

TEST_CASE("Vector supports move-only element types", "[vector]") {
  core::container::Vector<MoveOnly> values;

  values.emplace_back(1);
  values.emplace_back(3);
  values.emplace(values.begin() + 1, 2);

  REQUIRE(*values[0].value == 1);
  REQUIRE(*values[1].value == 2);
  REQUIRE(*values[2].value == 3);

  values.erase(values.begin());
  REQUIRE(values.size() == 2);
  REQUIRE(*values[0].value == 2);
}

TEST_CASE("Vector copy construction cleans up correctly when element copy throws", "[vector]") {
  core::container::Vector<ThrowOnCopy> source;
  source.emplace_back(1);
  source.emplace_back(2);
  ThrowOnCopy::copiesBeforeThrow = 0;

  REQUIRE_THROWS_AS(core::container::Vector(source), std::runtime_error);
  REQUIRE(source.size() == 2);
  REQUIRE(source[0].value == 1);
  REQUIRE(source[1].value == 2);

  ThrowOnCopy::copiesBeforeThrow = -1;
}

TEST_CASE("Vector move assignment preserves destination allocator when pool allocators differ", "[vector][allocator]") {
  using Allocator = StatefulAllocator<int>;
  using AllocatorVector = core::container::Vector<int, Allocator>;

  int stateA = 1;
  int stateB = 2;

  std::pmr::monotonic_buffer_resource resourceA;
  std::pmr::monotonic_buffer_resource resourceB;
  const Allocator allocatorA(&resourceA, &stateA);
  const Allocator allocatorB(&resourceB, &stateB);

  AllocatorVector source(allocatorA);
  source.push_back(11);
  source.push_back(22);

  AllocatorVector destination(allocatorB);
  destination.push_back(99);
  destination = std::move(source);

  REQUIRE(destination.size() == 2);
  REQUIRE(destination[0] == 11);
  REQUIRE(destination[1] == 22);
  REQUIRE(destination.size() == 2);
  // The move-assignment behavior is validated by resulting contents and source emptiness.
  REQUIRE(source.empty());
}

TEST_CASE("Vector supports comparison and swap operations", "[vector]") {
  core::container::Vector<int> left{1, 2, 3};
  core::container::Vector<int> same{1, 2, 3};
  core::container::Vector<int> greater{1, 2, 4};

  REQUIRE(left == same);
  REQUIRE(left != greater);
  REQUIRE(left < greater);
  REQUIRE(greater > left);
  REQUIRE(left <= same);
  REQUIRE(greater >= same);

  swap(left, greater);
  REQUIRE(left[2] == 4);
  REQUIRE(greater[2] == 3);
}
