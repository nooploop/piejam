// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <atomic>
#include <functional>
#include <type_traits>

namespace piejam::thread
{

template <class T>
class spsc_slot
{
public:
    void push(T const& v) noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        m_store[m_write] = v;
        commit();
    }

    void push(T&& v) noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        m_store[m_write] = std::move(v);
        commit();
    }

    template <class F>
    void consume(F&& f) noexcept(
            noexcept(std::invoke(std::forward<F>(f), std::declval<T>())))
    {
        if (marked(m_swap_pos.load()))
        {
            m_read = pos(m_swap_pos.exchange(m_read));
            std::invoke(std::forward<F>(f), m_store[m_read]);
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
        m_write = pos(m_swap_pos.exchange(mark(m_write)));
    }

    static auto mark(std::size_t x) -> std::size_t { return x | 0b100; }
    static bool marked(std::size_t x) { return (x & 0b100); }
    static auto pos(std::size_t x) -> std::size_t { return x & 0b11; }

    std::atomic_size_t m_swap_pos{1};
    std::size_t m_read{0};
    std::size_t m_write{2};
    std::array<T, 3> m_store;
};

} // namespace piejam::thread
