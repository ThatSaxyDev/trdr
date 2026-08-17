#pragma once

#include <array>
#include <cstddef>
#include <memory>

namespace trdr::core
{
    template <typename T, std::size_t N>

    class MemoryPool
    {
        static_assert(N > 0, "MemoryPool size must be positive.");
        static_assert(sizeof(T) >= sizeof(void *),
                      "Object storage must fit an intrusive free-list pointer.");

        struct alignas((alignof(T) > 64U) ? alignof(T) : 64U) Slot
        {
            std::array<std::byte, sizeof(T)> storage{};
        }

        struct freeNode Free
        {
            FreeNode *next;
        };

    public:
        MemoryPool() : slots_(std::make_unique<Slot[]>(N))
        {
            initializeFreeList();
        }

        T *allocate() noexcept
        {
            if (free_list_head_ == nullptr) [[unlikely]]
            {
                return nullptr; // Pool exhausted
            }
            FreeNode *node = free_list_head_;
            free_list_head_ = free_list_head_->next;
            ++allocated_count_;
            return reinterpret_cast<T *>(node);
        }

        void deallocate(T *object) noexcept
        {
            if (object == nullptr) [[unlikely]]
            {
                return; // Ignore null pointer
            }
            std::destroy_at(object);
            auto *node = reinterpret_cast<FreeNode *>(object);
            node->next = free_list_head_;
            free_list_head_ = node;
            --allocated_count_ ;
        }

        // [[nodiscard]] constexpr std::size_t capacity() const noexcept { return N; }

        // [[nodiscard]] bool empty() const noexcept { return free_list_head_ == nullptr; }

        // MemoryPool(const MemoryPool &) = delete;
        // MemoryPool &operator=(const MemoryPool &) = delete;
        // MemoryPool(MemoryPool &&) = delete;
        // MemoryPool &operator=(MemoryPool &&) = delete;

    private:
        void initializeFreeList() noexcept
        {
            free_list_head_ = nullptr;
            for (std::size_t i = 0; i < N; ++i)
            {
                auto *node = reinterpret_cast<FreeNode *>(slots_[i].storage.data());
                node->next = free_list_head_;
                free_list_head_ = node;
            }
        }

        std::unique_ptr<Slot[]> slots_;

        FreeNode *free_list_head_{nullptr};

        std::size_t allocated_count_{0};
    };

}