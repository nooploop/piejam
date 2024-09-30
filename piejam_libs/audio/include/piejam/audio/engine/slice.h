// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <concepts>
#include <functional>
#include <span>

namespace piejam::audio::engine
{

template <class T>
class slice
{
public:
    enum class kind : bool
    {
        constant,
        span,
    };

    using value_type = T;
    using constant_t = T;
    using span_t = std::span<T const>;

    constexpr slice() noexcept = default;

    constexpr slice(constant_t v) noexcept
        : m_kind{kind::constant}
        , m_value{.constant = v}
    {
    }

    template <std::convertible_to<span_t> U>
    constexpr slice(U&& u) noexcept(noexcept(span_t(std::forward<U>(u))))
        : m_kind{kind::span}
        , m_value{.span = span_t(std::forward<U>(u))}
    {
    }

    [[nodiscard]]
    constexpr auto is_constant() const noexcept -> bool
    {
        return m_kind == kind::constant;
    }

    [[nodiscard]]
    constexpr auto constant() const noexcept -> constant_t
    {
        BOOST_ASSERT(is_constant());
        return m_value.constant;
    }

    [[nodiscard]]
    constexpr auto is_span() const noexcept -> bool
    {
        return m_kind == kind::span;
    }

    [[nodiscard]]
    constexpr auto span() const noexcept -> span_t const&
    {
        BOOST_ASSERT(is_span());
        return m_value.span;
    }

    template <class Visitor>
    static auto visit(Visitor&& v, slice const s)
    {
        switch (s.m_kind)
        {
            case kind::constant:
                return std::invoke(v, s.m_value.constant);

            case kind::span:
                return std::invoke(v, s.m_value.span);
        }
    }

    template <class Visitor>
    static auto visit(Visitor&& v, slice const s1, slice const s2)
    {
        switch (s1.m_kind)
        {
            case kind::constant:
                switch (s2.m_kind)
                {
                    case kind::constant:
                        return std::invoke(
                                v,
                                s1.m_value.constant,
                                s2.m_value.constant);

                    case kind::span:
                        return std::invoke(
                                v,
                                s1.m_value.constant,
                                s2.m_value.span);
                }

            case kind::span:
                switch (s2.m_kind)
                {
                    case kind::constant:
                        return std::invoke(
                                v,
                                s1.m_value.span,
                                s2.m_value.constant);

                    case kind::span:
                        return std::invoke(v, s1.m_value.span, s2.m_value.span);
                }
        }
    }

private:
    kind m_kind{};
    union
    {
        constant_t constant{};
        span_t span;
    } m_value{};
};

} // namespace piejam::audio::engine
