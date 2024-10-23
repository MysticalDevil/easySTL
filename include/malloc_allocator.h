#pragma once

#ifndef EASYSTL_MALLOC_ALLOCATOR_H_
#define EASYSTL_MALLOC_ALLOCATOR_H_

#include <cstdlib>

namespace easystl {
/**
 * @class MallocAllocator
 * @brief A memory allocator that use `malloc`, `realloc`, and `free` for memory
 * management. This allocator allows fow the ise of a custom out-of-memory(OOM)
 * handler.
 */
class MallocAllocator {
public:
  /**
   * @brief Allocates memory of a specified size using `malloc`.
   * @param size The size of the memory block to allocate.
   * @return A pointer to the allocated memory block, or invokes an OOM handler
   * if `malloc` fails.
   */
  static auto Allocate(const std::size_t size) -> void * {
    void *result = malloc(size);
    if (result == nullptr) {
      result = MallocInOom(size);
    } // Call OOM handler if allocation fails
    return result;
  }

  /**
   * @brief Deallocates a memory block previously allocated with `Allocate`.
   * @param obj Pointer to the memory block to deallocate.
   * @param{unnamed} Unused parameter, present for compatibility with other
   * allocators.
   */
  static auto Deallocate(void *obj, std::size_t /*size*/) -> void { free(obj); }

  /**
   * @brief Reallocates a memory block to a new size using `realloc`.
   * @param obj Pointer to the memory block to reallocate.
   * @param{unnamed} The current size of the memory block (unused).
   * @param newsize The new size of the memory block.
   * @return A pointer to the reallocated memory block, or invokes an OOM
   * handler if `realloc` fails.
   */
  static auto Reallocate(void *obj, std::size_t /*oldsize*/,
                         const size_t newsize) -> void * {
    void *result = realloc(obj, newsize);
    if (result == nullptr) {
      result = ReallocInOom(obj, newsize);
    } // Call OOM handler if reallocation fails.
    return result;
  }

  /**
   * @brief Sets a custom out-of-memory (OOM) handler to be invoked when
   * allocation fails.
   * @param func The function to be set as the OOM handler.
   * @return The previous OOM handler.
   */
  static auto (*SetMallocHandler(void (*func)()))() {
    void (*old)() = CustomerOomHandler;
    CustomerOomHandler = func; // Set the new handler
    return old;                // return the previous handler
  }

private:
  /**
   * @brief Handles memory allocation failures by invoking the OOM handler.
   * @param size The size of memory to allocate.
   * @return A pointer to the allocated memory block, or aborts if OOM handler
   * cannot resolve the issue.
   */
  static auto MallocInOom(std::size_t size) -> void *;

  /**
   * @brief Handles memory reallocation failures by invoking the OOM handler.
   * @param obj Pointer to the existing memory block to reallocate.
   * @param size The new size of the memory block.
   * @return A pointer to the reallocated memory block, or aborts if OOM handler
   * cannot resolve the issue.
   */
  static auto ReallocInOom(void *obj, std::size_t size) -> void *;

  /**
   * @brief The user-defined OOM handler.
   * If memory allocation or reallocation fails, this handler is invoked.
   */
  static auto (*CustomerOomHandler)() -> void;
};

/**
 * @brief Initialize the OOM handler to `nullptr` by default.
 */
void (*MallocAllocator::CustomerOomHandler)() = nullptr;

/**
 * @brief Attempts to allocate memory when `malloc` fails, invoking the OOM
 * handler. If the handler is set, it will retry 3 times before returning
 * `nullptr` or aborting.
 * @param size The size of memory to allocate.
 * @return A pointer to the allocated memory, or `nullptr` if allocation fails
 * after retries.
 */
inline auto MallocAllocator::MallocInOom(const std::size_t size) -> void * {
  int retry_count = 0;
  constexpr int max_retries = 3;

  while (retry_count < max_retries) {
    void (*my_malloc_handler)() = CustomerOomHandler;
    // If no OOM handler is set, abort the program
    if (my_malloc_handler == nullptr) {
      std::abort();
    }
    (*my_malloc_handler)(); // Invoke the OOM handler
    if (void *result = malloc(size)) {
      return result;
    } // Try allocating memory again
    retry_count++;
  }
  return nullptr;
}

/**
 * @brief Attempts to reallocate memory when `realloc` fails, invoking the OOM
 * handler. If the handler is set, it will retry up to 3 times before returning
 * `nullptr` or aborting.
 * @param obj Pointer to the existing memory block to reallocate.
 * @param size The new size of the memory block.
 * @return A pointer to the reallocated memory, or `nullptr` if reallocation
 * fails after retries.
 */
inline auto MallocAllocator::ReallocInOom(void *obj, const std::size_t size)
    -> void * {
  int retry_count = 0;
  constexpr int max_retries = 3;

  while (retry_count < max_retries) {
    void (*my_malloc_handler)() = CustomerOomHandler;
    if (my_malloc_handler == nullptr) {
      std::abort();
    }
    (*my_malloc_handler)();
    if (void *result = realloc(obj, size)) {
      return result;
    }
    retry_count++;
  }
  return nullptr;
}
} // namespace easystl

#endif // !EASYSTL_MALLOC_ALLOCATOR_H_
