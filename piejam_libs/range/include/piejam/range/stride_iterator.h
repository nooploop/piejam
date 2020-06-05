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

#include <cassert>
#include <iterator>
#include <type_traits>

namespace piejam::range
{

template <class Iterator>
class stride_iterator
{
public:
    using iterator_category =
            typename std::iterator_traits<Iterator>::iterator_category;
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type =
            typename std::iterator_traits<Iterator>::difference_type;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using pointer = typename std::iterator_traits<Iterator>::pointer;

    constexpr stride_iterator() noexcept(
            std::is_nothrow_default_constructible_v<Iterator>) = default;

    constexpr stride_iterator(
            Iterator const& it,
            difference_type const
                    stride) noexcept(std::
                                             is_nothrow_copy_constructible_v<
                                                     Iterator>)
        : m_it(it)
        , m_stride(stride)
    {
    }

    constexpr stride_iterator(
            Iterator&& it,
            difference_type const
                    stride) noexcept(std::
                                             is_nothrow_move_constructible_v<
                                                     Iterator>)
        : m_it(std::move(it))
        , m_stride(stride)
    {
    }

    [[nodiscard]] constexpr auto stride() const noexcept { return m_stride; }

    [[nodiscard]] constexpr auto operator*() const noexcept -> reference
    {
        return *m_it;
    }

    [[nodiscard]] constexpr auto operator->() const noexcept -> pointer
    {
        return m_it.operator->();
    }

    [[nodiscard]] constexpr auto
    operator[](difference_type const n) const noexcept -> value_type
    {
        return m_it[n * m_stride];
    }

    constexpr auto operator++() noexcept -> stride_iterator&
    {
        std::advance(m_it, m_stride);
        return *this;
    }

    constexpr auto operator++(int) noexcept -> stride_iterator
    {
        auto copy = *this;
        std::advance(m_it, m_stride);
        return copy;
    }

    constexpr auto operator+=(difference_type const n) noexcept
            -> stride_iterator&
    {
        std::advance(m_it, n * m_stride);
        return *this;
    }

    constexpr auto operator--() noexcept -> stride_iterator&
    {
        std::advance(m_it, -m_stride);
        return *this;
    }

    constexpr auto operator--(int) noexcept -> stride_iterator
    {
        auto copy = *this;
        std::advance(m_it, -m_stride);
        return copy;
    }

    constexpr auto operator-=(difference_type const n) noexcept
            -> stride_iterator&
    {
        std::advance(m_it, -n * m_stride);
        return *this;
    }

    friend constexpr bool
    operator==(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        assert(lhs.m_stride == rhs.m_stride);
        assert(!lhs.m_stride || (lhs.m_it - rhs.m_it) % lhs.m_stride == 0);
        return lhs.m_it == rhs.m_it;
    }

    friend constexpr bool
    operator!=(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool
    operator<(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        assert(lhs.m_stride == rhs.m_stride);
        assert(!lhs.m_stride || (lhs.m_it - rhs.m_it) % lhs.m_stride == 0);
        return lhs.m_it < rhs.m_it;
    }

    friend constexpr bool
    operator<=(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        return lhs == rhs || lhs < rhs;
    }

    friend constexpr bool
    operator>(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        return rhs < lhs;
    }

    friend constexpr bool
    operator>=(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
    {
        return rhs <= lhs;
    }

    friend constexpr auto
    operator+(stride_iterator it, difference_type const n) noexcept
            -> stride_iterator
    {
        return it += n;
    }

    friend constexpr auto
    operator+(difference_type const n, stride_iterator const& it) noexcept
            -> stride_iterator
    {
        return it + n;
    }

    friend constexpr auto
    operator-(stride_iterator it, difference_type const n) noexcept
            -> stride_iterator
    {
        return it -= n;
    }

    friend constexpr auto
    operator-(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
            -> difference_type
    {
        assert(lhs.m_stride == rhs.m_stride);
        assert((lhs.m_it - rhs.m_it) % lhs.m_stride == 0);
        return (lhs.m_it - rhs.m_it) / lhs.m_stride;
    }

private:
    Iterator m_it{};
    difference_type m_stride{};
};

} // namespace piejam::range
