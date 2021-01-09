// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <concepts>
#include <span>
#include <variant>

namespace piejam::audio::engine
{

template <class T>
class slice
{
public:
    using span_t = std::span<T const>;
    using variant_t = std::variant<T, span_t>;

    constexpr slice() noexcept = default;
    constexpr slice(T v) noexcept
        : m_value(v)
    {
    }

    template <class U>
    constexpr slice(U&& u) requires std::convertible_to<U, span_t>
        : m_value(span_t(std::forward<U>(u)))
    {
    }

    constexpr bool is_constant() const noexcept
    {
        return std::holds_alternative<T>(m_value);
    }

    constexpr auto constant() const noexcept -> T
    {
        BOOST_ASSERT(is_constant());
        return *std::get_if<0>(&m_value);
    }

    constexpr bool is_buffer() const noexcept
    {
        return std::holds_alternative<span_t>(m_value);
    }

    constexpr auto buffer() const noexcept -> span_t const&
    {
        BOOST_ASSERT(is_buffer());
        return *std::get_if<1>(&m_value);
    }

    constexpr auto as_variant() const noexcept -> variant_t const&
    {
        return m_value;
    }

private:
    variant_t m_value{};
};

} // namespace piejam::audio::engine
