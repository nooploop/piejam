// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <concepts>
#include <functional>
#include <span>

namespace piejam::audio
{

enum class slice_kind : bool
{
    constant,
    span,
};

template <class T>
class slice
{
public:
    using value_type = T;
    using constant_t = T;
    using span_t = std::span<T const>;

    constexpr slice() noexcept = default;

    constexpr slice(constant_t v) noexcept
        : m_kind{slice_kind::constant}
        , m_value{.constant = v}
    {
    }

    template <std::convertible_to<span_t> U>
    constexpr slice(U&& u) noexcept(noexcept(span_t(std::forward<U>(u))))
        : m_kind{slice_kind::span}
        , m_value{.span = span_t(std::forward<U>(u))}
    {
    }

    [[nodiscard]]
    constexpr auto kind() const noexcept -> slice_kind
    {
        return m_kind;
    }

    [[nodiscard]]
    constexpr auto is_constant() const noexcept -> bool
    {
        return m_kind == slice_kind::constant;
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
        return m_kind == slice_kind::span;
    }

    [[nodiscard]]
    constexpr auto span() const noexcept -> span_t const&
    {
        BOOST_ASSERT(is_span());
        return m_value.span;
    }

private:
    slice_kind m_kind{};
    union
    {
        constant_t constant{};
        span_t span;
    } m_value{};
};

template <class Visitor, class T>
static auto
visit(Visitor&& v, slice<T> const s)
{
    switch (s.kind())
    {
        case slice_kind::constant:
            return std::invoke(std::forward<Visitor>(v), s.constant());

        case slice_kind::span:
            return std::invoke(std::forward<Visitor>(v), s.span());
    }
}

template <class Visitor, class T, class U>
static auto
visit(Visitor&& v, slice<T> const s1, slice<U> const s2)
{
    switch (s1.kind())
    {
        case slice_kind::constant:
            switch (s2.kind())
            {
                case slice_kind::constant:
                    return std::invoke(
                            std::forward<Visitor>(v),
                            s1.constant(),
                            s2.constant());

                case slice_kind::span:
                    return std::invoke(
                            std::forward<Visitor>(v),
                            s1.constant(),
                            s2.span());
            }

        case slice_kind::span:
            switch (s2.kind())
            {
                case slice_kind::constant:
                    return std::invoke(
                            std::forward<Visitor>(v),
                            s1.span(),
                            s2.constant());

                case slice_kind::span:
                    return std::invoke(
                            std::forward<Visitor>(v),
                            s1.span(),
                            s2.span());
            }
    }
}

} // namespace piejam::audio
