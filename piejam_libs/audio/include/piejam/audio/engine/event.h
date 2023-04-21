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
    static_assert(std::is_nothrow_default_constructible_v<T>);
    static_assert(std::is_nothrow_copy_constructible_v<T>);
    static_assert(std::is_nothrow_move_constructible_v<T>);

public:
    event() noexcept = default;

    event(std::size_t const offset, T const& value) noexcept
        : m_offset(offset)
        , m_value(value)
    {
    }

    event(std::size_t const offset, T&& value) noexcept
        : m_offset(offset)
        , m_value(std::move(value))
    {
    }

    [[nodiscard]] auto offset() const noexcept -> std::size_t
    {
        return m_offset;
    }

    [[nodiscard]] auto value() const noexcept -> T const&
    {
        return m_value;
    }

    [[nodiscard]] auto operator==(event const&) const -> bool = default;

private:
    std::size_t m_offset{};
    T m_value{};
};

} // namespace piejam::audio::engine
