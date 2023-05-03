// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <concepts>
#include <span>

namespace piejam::range
{

template <class T, std::size_t Extent = std::dynamic_extent>
class span_iterator
    : public boost::stl_interfaces::iterator_interface<
              span_iterator<T, Extent>,
              std::random_access_iterator_tag,
              std::span<T, Extent>>
{
public:
    using value_type = std::span<T, Extent>;

    constexpr span_iterator() noexcept = default;

    template <class U>
    constexpr span_iterator(span_iterator<U, Extent> const other) noexcept
        requires(std::is_convertible_v<U*, T*>)
        : m_span{other.m_span}
    {
    }

    template <class U, std::size_t ExtentOther>
    constexpr span_iterator(
            span_iterator<U, ExtentOther> const other) noexcept
        requires(std::is_convertible_v<U*, T*> && Extent == std::dynamic_extent)
        : m_span{other.m_span}
    {
    }

    template <class U>
    constexpr span_iterator(span_iterator<U> const other) noexcept
        requires(std::is_convertible_v<U*, T*> && Extent != std::dynamic_extent)
        : m_span{other.m_span.data(), other.m_span.size()}
    {
        BOOST_ASSERT(other.m_span.size() == Extent);
    }

    constexpr span_iterator(value_type const span) noexcept
        : m_span{span}
    {
    }

    [[nodiscard]] constexpr auto
    operator==(span_iterator const& other) const noexcept -> bool
    {
        return m_span.data() == other.m_span.data() &&
               m_span.size() == other.m_span.size();
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> value_type const&
    {
        return m_span;
    }

    constexpr auto operator+=(std::ptrdiff_t const n) noexcept
            -> span_iterator&
    {
        auto const span_size = m_span.size();
        BOOST_ASSERT(span_size != 0);
        m_span = value_type(m_span.data() + n * span_size, span_size);
        return *this;
    }

    constexpr auto operator-(span_iterator const& other) const noexcept
            -> std::ptrdiff_t
    {
        auto const span_size = m_span.size();
        BOOST_ASSERT(span_size != 0);
        BOOST_ASSERT((m_span.data() - other.m_span.data()) % span_size == 0);
        return (m_span.data() - other.m_span.data()) / span_size;
    }

private:
    template <class U, std::size_t>
    friend class span_iterator;

    value_type m_span{};
};

template <class T, std::size_t Extent>
span_iterator(std::span<T, Extent>) -> span_iterator<T, Extent>;

} // namespace piejam::range
