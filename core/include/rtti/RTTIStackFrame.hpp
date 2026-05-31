#pragma once

#include <cstring>
#include <new>

namespace core::rtti {
/**
 * @brief Represents a stack frame for invoking a function through the RTTI system, containing pointers for the "this"
 * object (if applicable), function arguments, and return value (if applicable).
 *
 * The layout of the stack frame is as follows:
 * - If the function is a non-static member function, the first pointer-sized slot is reserved for the "this" pointer.
 * - The next N pointer-sized slots are reserved for the function arguments, where N is the number of arguments the
 * function takes.
 * - If the function has a non-void return type, an additional pointer-sized slot is reserved at the end of the frame
 * for the return value.
 */
class RTTIStackFrame {
 public:
  /**
   * @brief Constructs a @c StackFrame with slots for arguments, return value, and optional @c this pointer.
   *
   * The constructor allocates a contiguous buffer and initializes each pointer slot to @c nullptr.
   * The destructor releases this buffer.
   *
   * @param numArgs The number of arguments in the function's signature.
   * @param hasReturn Whether the function has a non-void return value.
   * @param isMember Whether the function is a non-static member function that requires a "this" pointer.
   */
  explicit RTTIStackFrame(const std::size_t numArgs, const bool hasReturn, const bool isMember) noexcept
      : m_numArgs(numArgs), m_hasReturn(hasReturn), m_isMember(isMember) {
    m_buffer = operator new(pointerCount() * POINTER_SIZE, ALIGNMENT);
    memset(m_buffer, 0, pointerCount() * POINTER_SIZE);
  }

  /**
   * @brief Destructor for @c StackFrame, which deallocates the buffer used for storing the "this" pointer, argument
   * pointers, and return value pointer to prevent memory leaks.
   */
  ~RTTIStackFrame() {
    operator delete(m_buffer, pointerCount() * POINTER_SIZE, ALIGNMENT);
  }

  /**
   * @brief Returns a pointer to the "this" object for member function invocations, or @c nullptr if this stack frame
   * does not include a "this" pointer slot (i.e., for non-member functions).
   *
   * The caller is responsible for ensuring that the returned pointer is valid and points to an object of the correct
   * type for the member function being invoked.
   *
   * @tparam T The expected type of the "this" pointer, which can be specified for convenience when accessing the
   * pointer.
   * @return A pointer to the "this" object for member function invocations, or @c nullptr if this stack frame does not
   * include a "this" pointer slot.
   */
  template <typename T = void>
  [[nodiscard]] T* thisPtr() const noexcept {
    if (!m_isMember) {
      return nullptr;
    }
    return *static_cast<T**>(m_buffer);
  }

  /**
   * @brief Sets the "this" pointer for member function invocations in this stack frame. This method should only be
   * called if the stack frame includes a "this" pointer slot (i.e., for member functions). The caller is responsible
   * for ensuring that the provided pointer is valid and points to an object of the correct type for the member function
   * being invoked.
   *
   * @tparam T The type of the "this" pointer being set, which can be specified for convenience when setting the
   * pointer.
   * @param value A pointer to the object to be used as the "this" pointer for member function invocations in this stack
   * frame.
   */
  template <typename T = void>
  void thisPtr(T* value) noexcept {
    if (m_isMember) {
      *static_cast<void**>(m_buffer) = value;
    }
  }

  /**
   * @brief Returns a pointer to the argument at the specified index in the stack frame, or @c nullptr if the index is
   * out of bounds. The caller is responsible for ensuring that the returned pointer is valid and points to an object of
   * the correct type for the corresponding argument in the function signature.
   *
   * @tparam T The expected type of the argument pointer, which can be specified for convenience when accessing the
   * pointer.
   * @param index The zero-based index of the argument to retrieve, which must be less than the number of arguments
   * specified for this stack frame.
   * @return A pointer to the argument at the specified index in the stack frame, or @c nullptr if the index is out of
   * bounds.
   */
  template <typename T = void>
  T* argPtr(const std::size_t index) const noexcept {
    if (index >= m_numArgs) {
      return nullptr;
    }

    auto ptr = reinterpret_cast<uintptr_t>(m_buffer);

    if (m_isMember) {
      ptr += POINTER_SIZE;
    }

    return *reinterpret_cast<T**>(ptr + index * POINTER_SIZE);
  }

  /**
   * @brief Sets the argument pointer at the specified index in the stack frame.
   *
   * The call is ignored when @c index is out of range or @c value is @c nullptr.
   *
   * @tparam T The type of the argument pointer being set, which can be specified for convenience when setting the
   * pointer.
   * @param index The zero-based index of the argument to set, which must be less than the number of arguments specified
   * for this stack frame.
   * @param value A pointer to the object to be used as the argument at the specified index in the stack frame.
   */
  template <typename T = void>
  void argPtr(const std::size_t index, T* value) noexcept {
    if (index < m_numArgs && value != nullptr) {
      auto ptr = reinterpret_cast<uintptr_t>(m_buffer);

      if (m_isMember) {
        ptr += POINTER_SIZE;
      }

      *reinterpret_cast<void**>(ptr + index * POINTER_SIZE) = value;
    }
  }

  /**
   * @brief Returns a pointer to the return value slot in the stack frame, or @c nullptr if this stack frame does not
   * include a return value slot (i.e., for functions with a void return type).
   *
   * The caller is responsible for ensuring that the returned pointer is valid and points to an object of the correct
   * type for the return value of the function being invoked.
   *
   * @tparam T The expected type of the return value pointer, which can be specified for convenience when accessing the
   * pointer.
   * @return A pointer to the return value slot in the stack frame, or @c nullptr if this stack frame does not include a
   * return value slot.
   */
  template <typename T = void>
  T* returnPtr() const noexcept {
    if (!m_hasReturn) {
      return nullptr;
    }

    auto ptr = reinterpret_cast<uintptr_t>(m_buffer);

    if (m_isMember) {
      ptr += POINTER_SIZE;
    }

    ptr += m_numArgs * POINTER_SIZE;

    return *reinterpret_cast<T**>(ptr);
  }

  /**
   * @brief Sets the return value pointer in the stack frame to the provided value. This method should only be called if
   * the stack frame includes a return value slot (i.e., for functions with a non-void return type).
   *
   * The caller is responsible for ensuring that the provided pointer is valid and points to an object of the correct
   * type for the return value of the function being invoked.
   *
   * @tparam T The type of the return value pointer being set, which can be specified for convenience when setting the
   * pointer.
   * @param value A pointer to the object to be used as the return value for function invocations in this stack frame.
   */
  template <typename T = void>
  void returnPtr(T* value) noexcept {
    if (m_hasReturn && value != nullptr) {
      auto ptr = reinterpret_cast<uintptr_t>(m_buffer);

      if (m_isMember) {
        ptr += POINTER_SIZE;
      }

      ptr += m_numArgs * POINTER_SIZE;

      *reinterpret_cast<void**>(ptr) = value;
    }
  }

 private:
  /**
   * @brief The size of a pointer on the target platform, which determines the size of each slot in the stack frame for
   * the "this" pointer, argument pointers, and return value pointer.
   *
   */
  static constexpr auto POINTER_SIZE = sizeof(void*);

  /**
   * @brief The alignment requirement for the stack frame buffer, which is set to the alignment of a pointer on the
   * target platform to ensure that all pointer accesses are properly aligned.
   */
  static constexpr auto ALIGNMENT = static_cast<std::align_val_t>(alignof(void*));

  /**
   * @brief Calculates the total pointer-slot count required for this frame.
   *
   * This value includes argument slots plus optional slots for @c this and return storage.
   *
   * @return This function returns the total pointer-slot count required for this frame.
   */
  [[nodiscard]] std::size_t pointerCount() const noexcept {
    return m_numArgs + (m_hasReturn ? 1 : 0) + (m_isMember ? 1 : 0);
  }

  /**
   * @brief The number of argument slots in the stack frame, which determines how many argument pointers can be stored.
   */
  std::size_t m_numArgs;

  /**
   * @brief Whether the stack frame includes a slot for a return value pointer, which is true if the function has a
   * non-void return type. This flag determines whether space is allocated in the buffer for a return value pointer.
   */
  bool m_hasReturn;

  /**
   * @brief Whether the stack frame includes a slot for a "this" pointer, which is true if the function is a non-static
   * member function. This flag determines whether space is allocated in the buffer for a "this" pointer.
   */
  bool m_isMember;

  /**
   * @brief A pointer to a contiguous block of memory that serves as the storage for the "this" pointer (if applicable),
   * argument pointers, and return value pointer (if applicable) for the stack frame. The layout of the buffer is as
   * follows:
   * - If m_isMember is true, the first pointer-sized slot is reserved for the "this" pointer.
   * - The next @c m_numArgs pointer-sized slots are reserved for argument pointers set and read via @c argPtr.
   */
  void* m_buffer;
};
}  // namespace core::rtti
