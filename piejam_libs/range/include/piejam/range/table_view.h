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

#include <piejam/range/strided_span.h>

#include <algorithm>
#include <cassert>

namespace piejam::range
{

template <class T>
class table_view
{
public:
    template <class U>
    class major_index_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = strided_span<U>;
        using difference_type = std::ptrdiff_t;
        using pointer = strided_span<U> const*;
        using reference = strided_span<U> const&;

        constexpr major_index_iterator() noexcept = default;
        constexpr major_index_iterator(
                U* const data,
                difference_type const step,
                std::size_t const minor_size,
                difference_type const minor_step) noexcept
            : m_stride(data, minor_size, minor_step)
            , m_step(step)
        {
        }

        constexpr auto operator*() const noexcept -> reference
        {
            return m_stride;
        }
        constexpr auto operator->() const noexcept -> pointer
        {
            return &m_stride;
        }

        constexpr auto operator[](difference_type const n) const noexcept
                -> value_type
        {
            return {m_stride.data() + n * m_step,
                    m_stride.size(),
                    m_stride.stride()};
        }

        constexpr auto operator++() noexcept -> major_index_iterator<U>&
        {
            m_stride = {
                    m_stride.data() + m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return *this;
        }

        constexpr auto operator++(int) noexcept -> major_index_iterator<U>
        {
            auto copy = *this;
            m_stride = {
                    m_stride.data() + m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return copy;
        }

        constexpr auto operator+=(difference_type const n) noexcept
                -> major_index_iterator<U>&
        {
            m_stride = {
                    m_stride.data() + n * m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return *this;
        }

        constexpr auto operator--() noexcept -> major_index_iterator<U>&
        {
            m_stride = {
                    m_stride.data() - m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return *this;
        }

        constexpr auto operator--(int) noexcept -> major_index_iterator<U>
        {
            auto copy = *this;
            m_stride = {
                    m_stride.data() - m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return copy;
        }

        constexpr auto operator-=(difference_type const n) noexcept
                -> major_index_iterator<U>&
        {
            m_stride = {
                    m_stride.data() - n * m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return *this;
        }

        friend constexpr bool operator==(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            assert(lhs.m_stride.size() == rhs.m_stride.size());
            assert(lhs.m_stride.stride() == rhs.m_stride.stride());
            assert(lhs.m_step == rhs.m_step);
            return lhs.m_stride.data() == rhs.m_stride.data();
        }

        friend constexpr bool operator!=(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        friend constexpr bool operator<(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            assert(lhs.m_stride.size() == rhs.m_stride.size());
            assert(lhs.m_stride.stride() == rhs.m_stride.stride());
            assert(lhs.m_step == rhs.m_step);
            return lhs.m_stride.data() < rhs.m_stride.data();
        }

        friend constexpr bool operator<=(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            return lhs == rhs || lhs < rhs;
        }

        friend constexpr bool operator>(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            return rhs < lhs;
        }

        friend constexpr bool operator>=(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept
        {
            return rhs <= lhs;
        }

        friend constexpr auto
        operator+(major_index_iterator<U> it, difference_type const n) noexcept
                -> major_index_iterator<U>
        {
            return it += n;
        }

        friend constexpr auto operator+(
                difference_type const n,
                major_index_iterator<U> const& it) noexcept
                -> major_index_iterator<U>
        {
            return it + n;
        }

        friend constexpr auto
        operator-(major_index_iterator<U> it, difference_type const n) noexcept
                -> major_index_iterator<U>
        {
            return it -= n;
        }

        friend constexpr auto operator-(
                major_index_iterator<U> const& lhs,
                major_index_iterator<U> const& rhs) noexcept -> difference_type
        {
            assert(lhs.m_stride.size() == rhs.m_stride.size());
            assert(lhs.m_stride.stride() == rhs.m_stride.stride());
            assert(lhs.m_step == rhs.m_step);
            return (lhs.m_stride.data() - rhs.m_stride.data()) / lhs.m_step;
        }

    private:
        strided_span<U> m_stride;
        difference_type m_step;
    };

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = major_index_iterator<T>;
    using const_iterator = major_index_iterator<T const>;

    constexpr table_view() noexcept = default;
    constexpr table_view(
            T* const data,
            size_type const major_size,
            size_type const minor_size,
            difference_type const major_step,
            difference_type const minor_step) noexcept
        : m_data(data)
        , m_major_size(major_size)
        , m_minor_size(minor_size)
        , m_major_step(major_step)
        , m_minor_step(minor_step)
    {
    }

    constexpr auto data() const noexcept -> T* { return m_data; }
    constexpr auto major_size() const noexcept -> size_type
    {
        return m_major_size;
    }
    constexpr auto minor_size() const noexcept -> size_type
    {
        return m_minor_size;
    }
    constexpr auto major_step() const noexcept -> difference_type
    {
        return m_major_step;
    }
    constexpr auto minor_step() const noexcept -> difference_type
    {
        return m_minor_step;
    }

    constexpr auto begin() const noexcept -> iterator
    {
        return {m_data, m_major_step, m_minor_size, m_minor_step};
    }

    constexpr auto cbegin() const noexcept -> const_iterator
    {
        return {m_data, m_major_step, m_minor_size, m_minor_step};
    }

    constexpr auto end() const noexcept -> iterator
    {
        return {m_data + m_major_size * m_major_step,
                m_major_step,
                m_minor_size,
                m_minor_step};
    }

    constexpr auto cend() const noexcept -> const_iterator
    {
        return {m_data + m_major_size * m_major_step,
                m_major_step,
                m_minor_size,
                m_minor_step};
    }

    constexpr auto operator[](size_type major_index) const noexcept
            -> strided_span<T>
    {
        return {m_data + major_index * m_major_step,
                m_minor_size,
                m_minor_step};
    }

private:
    T* m_data;
    size_type m_major_size;
    size_type m_minor_size;
    difference_type m_major_step;
    difference_type m_minor_step;
};

template <class T>
auto
transpose(table_view<T> const& t) noexcept -> table_view<T>
{
    return {t.data(),
            t.minor_size(),
            t.major_size(),
            t.minor_step(),
            t.major_step()};
}

template <class T, class U, class UnaryOperation>
auto
transform(
        table_view<T> const& src,
        table_view<U> const& dst,
        UnaryOperation&& op)
{
    assert(src.major_size() == dst.major_size());
    assert(src.minor_size() == dst.minor_size());

    for (auto src_it = src.begin(), src_end = src.end(), dst_it = dst.begin();
         src_it != src_end;
         ++src_it, ++dst_it)
        std::transform(src_it->begin(), src_it->end(), dst_it->begin(), op);
}

template <class T>
void
fill(table_view<T> const& tv, T value)
{
    for (auto row : tv)
        std::fill(row.begin(), row.end(), value);
}

} // namespace piejam::range
