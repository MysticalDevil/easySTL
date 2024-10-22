#pragma once

#ifndef EASYSTL_MEMORY_POOL_ALLOCATOR_H_
#define EASYSTL_MEMORY_POOL_ALLOCATOR_H_

#include <malloc_allocator.h>
#include <memory_pool_allocator.h>

namespace easystl {
    /**
    * @class MemoryPoolList
    * @brief A linked list used for managing free nodes in the memory pool.
    * Each node contains a pointer to the next node, facilitating the allocation and deallocation of memory.
    */
    class MemoryPoolList {
    public:
        /**
        * @brief Checks if the list is empty.
        * @return `true` if the list is empty, `false` otherwise.
        */
        [[nodiscard]] auto Empty() const -> bool { return node_ == nullptr; }
        /**
        * @brief Retrieves the next node in the list.
        * @param node Pointer to the current node.
        * @return Reference to the next node pointer.
        */
        static auto GetNextNode(void *node) -> void *& { return *static_cast<void **>(node); }

        /**
        * @brief Pushed a node onto the front of the list.
        * @param node Pointer to the node to be added.
        */
        auto Push(void *node) -> void {
            GetNextNode(node) = node_;
            node_ = node;
        }

        /**
        * @brief Pops a node from the front of the list.
        * @return Pointer to the node that was removed from the list.
        */
        auto Pop() -> void * {
            void *result = node_;
            node_ = GetNextNode(result);
            return result;
        }

    private:
        void *node_ = nullptr; ///< Pointer to the head of the linked list.
    };

    // Constants for memory alignment and size limits.
    static constexpr int kAlign = 8; ///< Alignment size (in bytes).
    static constexpr int kMaxBytes = 128; ///< Maximum size of memory blocks managed by the pool.
    static constexpr int kFreeListNum = kMaxBytes / kAlign; ///< Number of free lists based on size.

    /**
    * @class MemoryPoolAllocator
    * @brief A memory allocator that uses a memory pool to efficiently manage small memory allocations.
    * It provides faster allocations for small objects by maintaining a set of free lists.
    */
    class MemoryPoolAllocator {
    public:
        /**
        * @brief Allocates memory of a specified size.
        * @param size The size of the memory block to allocate.
        * @return A pointer to the allocated memory block, or calls the MallocAllocator if size exceeds kMaxBytes.
        */
        static auto Allocate(const std::size_t size) -> void * {
            if (size > static_cast<std::size_t>(kMaxBytes)) {
                return MallocAllocator::Allocate(size); // Use MallocAllocator for larger sizes.
            }
            const std::size_t index = GetFreelistIndex(size);
            if (freelist_[index].Empty()) {
                return Refill(RoundUp(size)); // Refill the free list if empty.
            }
            return freelist_[index].Pop(); // Pop a node from the free list.
        }

        /**
        * @brief Deallocates a memory block previously allocated with Allocate.
        * @param obj Pointer to the memory block to deallocate.
        * @param size The size of the memory block being deallocated.
        */
        static auto Deallocate(void *obj, const std::size_t size) -> void {
            if (size > kMaxBytes) {
                MallocAllocator::Deallocate(obj, size); // Use MallocAllocator for larger size.
                return;
            }
            freelist_[GetFreelistIndex(size)].Push(obj); // Push the block back to the free list.
        }

        /**
        * @brief Reallocates a memory block to a new size.
        * @param obj Pointer to the memory block to reallocate.
        * @param obj Pointer to the memory block to reallocate.
        * @param oldsize The current size of the memory block.
        * @param newsize The new size of the memory block.
        * @return A pointer to the reallocated memory block.
        */
        static auto Reallocate(void *obj, const std::size_t oldsize, const std::size_t newsize) -> void * {
            if (RoundUp(newsize) == RoundUp(oldsize)) {
                return obj;
            }
            Deallocate(obj, oldsize); // Deallocate the old memory.
            return Allocate(newsize); // Allocate new memory.
        }

    private:
        /**
        * @brief Rounds up the size to the nearest multiple of kAlign.
        * @param bytes The size of round up.
        * @return The rounded size.
        */
        static auto RoundUp(const std::size_t bytes) -> std::size_t { return (bytes + kAlign - 1) & ~(kAlign - 1); }

        /**
        * @brief Gets the index of the free list based on the size of the block.
        * @param bytes The size of the memory block.
        * @return The index of the corresponding free list.
        */
        static auto GetFreelistIndex(const std::size_t bytes) -> std::size_t {
            return (bytes + kAlign - 1) / kAlign - 1;
        }

        /**
        * @brief Fills a free list by allocating memory for a given size.
        * @param size The size of memory to allocate.
        * @return Pointer to the allocated memory.
        */
        static auto Refill(std::size_t size) -> void *;

        /**
        * @brief Allocates a chunk of memory from the pool.
        * @param size The size of each block.
        * @param chunknums The number of blocks to allocate.
        * @return Pointer to the allocated chunk of memory.
        */
        static auto ChunkAlloc(std::size_t size, std::size_t &chunknums) -> char *;

        static MemoryPoolList freelist_[kFreeListNum]; ///< Array of free lists for different sizes.
        static char *freespacestart_; ///< Pointer to the start of the free space.
        static char *freespaceend_; ///< Pointer to the end of the free space.
        static std::size_t mallocoffset_; ///< Offset for memory allocation tracking.
    };

    // Static member variable initializations
    inline char *MemoryPoolAllocator::freespacestart_ = nullptr;
    inline char *MemoryPoolAllocator::freespaceend_ = nullptr;
    inline std::size_t MemoryPoolAllocator::mallocoffset_ = 0;
    inline MemoryPoolList MemoryPoolAllocator::freelist_[kFreeListNum];

    inline auto MemoryPoolAllocator::ChunkAlloc(const std::size_t size, std::size_t &chunknums) -> char * {
        char *result;
        std::size_t bytesneed = size * chunknums;
        const std::size_t bytesleft = freespaceend_ - freespacestart_;
        if (bytesleft >= bytesneed) {
            result = freespacestart_; // Enough space available
            freespacestart_ += bytesneed; // Update start pointer
            return result;
        }
        if (bytesleft >= size) {
            chunknums = bytesleft / size; // Adjust number of chunks
            bytesneed = chunknums * size; // Calculate total bytes needed
            result = freespacestart_;
            freespacestart_ += bytesneed; // Update start pointer
            return result;
        }

        // Allocate a larger chunk if not enough space is left.
        const std::size_t bytesget = bytesneed * 2 + RoundUp(mallocoffset_ >> 4);
        if (bytesleft >= kAlign) {
            freelist_[GetFreelistIndex(bytesleft)].Push(freespacestart_); // Push remaining space to freelist.
        }
        freespacestart_ = static_cast<char *>(malloc(bytesget)); // Allocate new chunk
        if (freespacestart_ == nullptr) {
            // Try to get space from freelists if allocation fails
            for (std::size_t i = size; i <= kMaxBytes; i += kAlign) {
                if (!freelist_[GetFreelistIndex(i)].Empty()) {
                    freespacestart_ = static_cast<char *>(freelist_[GetFreelistIndex(i)].Pop());
                    freespaceend_ = freespacestart_ + i;
                    return ChunkAlloc(size, chunknums); // Retry allocation
                }
            }
            freespaceend_ = nullptr;
            freespacestart_ = static_cast<char *>(MallocAllocator::Allocate(bytesget)); // Fallback to MallocAllocator
        }
        mallocoffset_ += bytesget; // Update offset for tracking
        freespaceend_ = freespacestart_ + bytesget; // Update the end pointer
        return ChunkAlloc(size, chunknums); // Retry allocation
    }

    inline auto MemoryPoolAllocator::Refill(const std::size_t size) -> void * {
        std::size_t chunknums = 20;
        char *chunk = ChunkAlloc(size, chunknums);
        char *nextchunk = chunk + size;
        if (chunknums == 1) { return chunk; }
        for (int i = 1; i < static_cast<int>(chunknums); i++) {
            freelist_[GetFreelistIndex(size)].Push(nextchunk);
            nextchunk += size;
        }
        return chunk;
    }
} // namespace easystl

#endif // !EASYSTL_MEMORY_POOL_ALLOCATOR_H_
