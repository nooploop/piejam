// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/intrusive/set_hook.hpp>

#include <type_traits>

namespace piejam::audio::engine
{

template <class T>
class event final : public boost::intrusive::set_base_hook<>
{
    static_assert(std::is_trivially_destructible_v<T>);

public:
    event() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
    event(std::size_t const offset,
          T const& value) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : m_offset(offset)
        , m_value(value)
    {
    }
    event(std::size_t const offset,
          T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : m_offset(offset)
        , m_value(std::move(value))
    {
    }

    auto offset() const noexcept -> std::size_t { return m_offset; }
    auto value() const noexcept -> T const& { return m_value; }

private:
    std::size_t m_offset{};
    T m_value{};
};

template <class T>
bool
operator==(event<T> const& l, event<T> const& r)
{
    return l.offset() == r.offset() && l.value() == r.value();
}

template <class T>
bool
operator!=(event<T> const& l, event<T> const& r)
{
    return !(l == r);
}

} // namespace piejam::audio::engine
