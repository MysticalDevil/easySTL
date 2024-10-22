#pragma once

#ifndef EASYSTL_MEMORY_POOL_ALLOCATOR_H_
#define EASYSTL_MEMORY_POOL_ALLOCATOR_H_

#include <malloc_allocator.h>
#include <memory_pool_allocator.h>

namespace easystl {
    // memory pool class in order to save space
    // every node will itself to be the pointer to the next
    class MemoryPoolList {
    public:
        [[nodiscard]] auto Empty() const -> bool { return node_ == nullptr; }
        static auto GetNextNode(void *node) -> void *& { return *static_cast<void **>(node); }

        auto Push(void *node) -> void {
            GetNextNode(node) = node_;
            node_ = node;
        }

        auto Pop() -> void * {
            void *result = node_;
            node_ = GetNextNode(result);
            return result;
        }

    private:
        void *node_ = nullptr;
    };

    // constexpr value
    static constexpr int kAlign = 8;
    static constexpr int kMaxBytes = 128;
    static constexpr int kFreeListNum = kMaxBytes / kAlign;

    // memory pool allocator
    class MemoryPoolAllocator {
    public:
        static auto Allocate(const std::size_t size) -> void * {
            if (size > static_cast<std::size_t>(kMaxBytes)) {
                return MallocAllocator::Allocate(size);
            }
            const std::size_t index = GetFreelistIndex(size);
            if (freelist_[index].Empty()) {
                return ReFill(RoundUp(size));
            }
            return freelist_[index].Pop();
        }

        static auto Deallocate(void *obj, const std::size_t size) -> void {
            if (size > kMaxBytes) {
                MallocAllocator::Deallocate(obj, size);
                return;
            }
            freelist_[GetFreelistIndex(size)].Push(obj);
        }

        static auto Reallocate(void *obj, const std::size_t oldsize, const std::size_t newsize) -> void * {
            if (RoundUp(newsize) == RoundUp(oldsize)) {
                return obj;
            }
            Deallocate(obj, oldsize);
            return Allocate(newsize);
        }

    private:
        // align size to multiples of 8
        static auto RoundUp(const std::size_t bytes) -> std::size_t { return (bytes + kAlign - 1) & ~(kAlign - 1); }

        static auto GetFreelistIndex(const std::size_t bytes) -> std::size_t {
            return (bytes + kAlign - 1) / kAlign - 1;
        }

        // to big, define outside refill free space
        static auto ReFill(std::size_t size) -> void *;

        // get new chunk to free space
        static auto ChunkAlloc(std::size_t size, std::size_t &chunknums) -> char *;

        static MemoryPoolList freelist_[kFreeListNum];
        static char *freespacestart_;
        static char *freespaceend_;
        static std::size_t mallocoffset_;
    };

    inline char *MemoryPoolAllocator::freespacestart_ = nullptr;
    inline char *MemoryPoolAllocator::freespaceend_ = nullptr;
    inline std::size_t MemoryPoolAllocator::mallocoffset_ = 0;
    inline MemoryPoolList MemoryPoolAllocator::freelist_[kFreeListNum];

    inline auto MemoryPoolAllocator::ChunkAlloc(const std::size_t size, std::size_t &chunknums) -> char * {
        char *result;
        std::size_t bytesneed = size * chunknums;
        std::size_t bytesleft = freespaceend_ - freespacestart_;
        if (bytesleft >= bytesneed) {
            result = freespacestart_;
            freespacestart_ += bytesneed;
            return result;
        }
        if (bytesleft >= size) {
            chunknums = bytesleft / size;
            bytesneed = chunknums * size;
            result = freespacestart_;
            freespacestart_ += bytesneed;
            return result;
        }

        const std::size_t bytesget = bytesneed * 2 + RoundUp(mallocoffset_ >> 4);
        if (bytesleft >= kAlign) {
            freelist_[GetFreelistIndex(bytesleft)].Push(freespacestart_);
        }
        freespacestart_ = static_cast<char *>(malloc(bytesget));
        if (freespacestart_ == nullptr) {
            for (std::size_t i = size; i <= kMaxBytes; i += kAlign) {
                if (!freelist_[GetFreelistIndex(i)].Empty()) {
                    freespacestart_ = static_cast<char *>(freelist_[GetFreelistIndex(i)].Pop());
                    freespaceend_ = freespacestart_ + i;
                    return ChunkAlloc(size, chunknums);
                }
            }
            freespaceend_ = nullptr;
            freespacestart_ = static_cast<char *>(MallocAllocator::Allocate(bytesget));
        }
        mallocoffset_ += bytesget;
        freespaceend_ = freespacestart_ + bytesget;
        return ChunkAlloc(size, chunknums);
    }

    inline auto MemoryPoolAllocator::ReFill(const std::size_t size) -> void * {
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
