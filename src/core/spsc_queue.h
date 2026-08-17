#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace trdr::core
{
    template <typename T, std::size_t N>
    class SPSCQueue
    {
        static_assert(N > 1, "SPSCQueue requires at least two slots.");
        static_assert((N & (N - 1U)) == 0U, "SPSCQueue size must be a power of two.");
        static_assert(std::is_trivially_copyable<T>::value, "SPSCQueue requires trivially copyable types.");

        struct alignas(128) Cursor
        {
            std::atomic<std::uint64_t> value{0};
        };

    public:
        bool push(const T &value) noexcept
        {
            const std::uint16_t tail = tail_.value.load(std::memory_order_relaxed);
            const std::uint16_t head = head_.value.load(std::memory_order_acquire);

            if ((tail - head) == N) [[unlikely]]
            {
                return false; // Queue is full
            }
            buffer_[tail & MASK] = value;
            tail_.value.store(tail + 1U, std::memory_order_release);
            return true;
        }

        bool pop(T &value) noexcept
        {
            const std::uint16_t head = head_.value.load(std::memory_order_relaxed);
            const std::uint16_t tail = tail_.value.load(std::memory_order_acquire);

            if (head == tail) [[unlikely]]
            {
                return false; // Queue is empty
            }
            value = buffer_[head & MASK];
            head_.value.store(head + 1U, std::memory_order_release);
            return true;
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return head_.value.load(std::memory_order_acquire) == tail_.value.load(std::memory_order_acquire);
        }

    private:
        static constexpr std::size_t MASK = N - 1U;
        std::array<T, N> buffer_{};
        Cursor head_{};
        Cursor tail_{};
    };
}
