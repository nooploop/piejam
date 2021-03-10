// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/cache_line_size.h>

#include <array>
#include <atomic>
#include <functional>
#include <type_traits>

namespace piejam::thread
{

template <class T>
class spsc_slot
{
    static_assert(std::atomic_size_t::is_always_lock_free);

public:
    void push(T const& v) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        m_store[m_write].value = v;
        commit();
    }

    void push(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        m_store[m_write].value = std::move(v);
        commit();
    }

    template <class F>
    void consume(F&& f) noexcept(
            noexcept(std::invoke(std::forward<F>(f), std::declval<T>())))
    {
        if (marked(m_swap_pos.load(std::memory_order_relaxed)))
        {
            m_read =
                    pos(m_swap_pos.exchange(m_read, std::memory_order_acq_rel));
            std::invoke(std::forward<F>(f), m_store[m_read].value);
        }
    }

    bool pull(T& r) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        bool pulled{};
        consume([&r, &pulled](T const& value) {
            r = value;
            pulled = true;
        });

        return pulled;
    }

private:
    void commit() noexcept
    {
        m_write = pos(
                m_swap_pos.exchange(mark(m_write), std::memory_order_acq_rel));
    }

    static auto mark(std::size_t x) -> std::size_t { return x | 0b100; }
    static bool marked(std::size_t x) { return (x & 0b100); }
    static auto pos(std::size_t x) -> std::size_t { return x & 0b11; }

    alignas(cache_line_size) std::atomic_size_t m_swap_pos{1};
    alignas(cache_line_size) std::size_t m_read{0};
    alignas(cache_line_size) std::size_t m_write{2};

    struct item
    {
        alignas(cache_line_size) T value;
    };

    std::array<item, 3> m_store;
};

} // namespace piejam::thread
