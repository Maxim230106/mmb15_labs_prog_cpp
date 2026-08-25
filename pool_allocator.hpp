#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T, std::size_t ChunkSize>
class PoolAllocator {
    static_assert(ChunkSize > 0, "ChunkSize must be greater than zero");

private:
    struct FreeNode {
        FreeNode* next;
    };

    using SlotStorage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    union Slot {
        FreeNode free_node;
        SlotStorage storage;
    };

    struct LargeAllocation {
        void* pointer;
        std::size_t count;
    };

    struct Storage {
        FreeNode* free_list = nullptr;
        std::size_t free_slots = 0;
        std::vector<void*> blocks;
        std::vector<LargeAllocation> large_blocks;

        ~Storage() {
            for (void* block : blocks) {
                ::operator delete(block);
            }

            for (const LargeAllocation& allocation : large_blocks) {
                ::operator delete(allocation.pointer);
            }
        }

        void reserve(std::size_t count) {
            while (free_slots < count) {
                add_block(count - free_slots);
            }
        }

        void* acquire_slot() {
            if (free_list == nullptr) {
                add_block(1);
            }

            FreeNode* slot = free_list;
            free_list = free_list->next;
            --free_slots;

            return slot;
        }

        void release_slot(void* pointer) noexcept {
            FreeNode* slot = static_cast<FreeNode*>(pointer);
            slot->next = free_list;
            free_list = slot;
            ++free_slots;
        }

        void* allocate_large(std::size_t count) {
            void* block = ::operator new(count * sizeof(T));
            large_blocks.push_back({block, count});
            return block;
        }

        void deallocate_large(void* pointer) noexcept {
            auto it = std::find_if(
                large_blocks.begin(),
                large_blocks.end(),
                [pointer](const LargeAllocation& allocation) {
                    return allocation.pointer == pointer;
                }
            );

            if (it == large_blocks.end()) {
                return;
            }

            ::operator delete(it->pointer);
            large_blocks.erase(it);
        }

        void add_block(std::size_t min_slots) {
            const std::size_t slot_count = std::max(ChunkSize, min_slots);
            Slot* block = static_cast<Slot*>(::operator new(slot_count * sizeof(Slot)));
            blocks.push_back(block);

            for (std::size_t index = 0; index < slot_count; ++index) {
                block[index].free_node.next = free_list;
                free_list = &block[index].free_node;
            }

            free_slots += slot_count;
        }
    };

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::false_type;

    template<typename U>
    struct rebind {
        using other = PoolAllocator<U, ChunkSize>;
    };

    PoolAllocator()
        : storage_(std::make_shared<Storage>()) {
    }

    PoolAllocator(const PoolAllocator&) noexcept = default;
    PoolAllocator(PoolAllocator&&) noexcept = default;
    PoolAllocator& operator=(const PoolAllocator&) noexcept = default;
    PoolAllocator& operator=(PoolAllocator&&) noexcept = default;

    template<typename U>
    PoolAllocator(const PoolAllocator<U, ChunkSize>&)
        : storage_(std::make_shared<Storage>()) {
    }

    T* allocate(std::size_t count) {
        if (count == 0) {
            return nullptr;
        }

        if (count == 1) {
            return static_cast<T*>(storage_->acquire_slot());
        }

        return static_cast<T*>(storage_->allocate_large(count));
    }

    void deallocate(T* pointer, std::size_t count) noexcept {
        if (pointer == nullptr) {
            return;
        }

        if (count == 1) {
            storage_->release_slot(pointer);
            return;
        }

        storage_->deallocate_large(pointer);
    }

    void reserve(std::size_t count) {
        storage_->reserve(count);
    }

    template<typename U>
    bool operator==(const PoolAllocator<U, ChunkSize>& other) const noexcept {
        return storage_.get() == other.storage_.get();
    }

    template<typename U>
    bool operator!=(const PoolAllocator<U, ChunkSize>& other) const noexcept {
        return !(*this == other);
    }

private:
    template<typename, std::size_t>
    friend class PoolAllocator;

    std::shared_ptr<Storage> storage_;
};
