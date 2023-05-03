// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/strided_span.h>

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <algorithm>

namespace piejam::range
{

template <class T>
class table_view
{
public:
    template <class U>
    class major_index_iterator
        : public boost::stl_interfaces::iterator_interface<
                  major_index_iterator<U>,
                  std::random_access_iterator_tag,
                  strided_span<U>,
                  strided_span<U> const&,
                  strided_span<U> const*>
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

        [[nodiscard]] constexpr auto
        operator==(major_index_iterator const& other) const noexcept -> bool
        {
            return m_stride.data() == other.m_stride.data() &&
                   m_stride.size() == other.m_stride.size() &&
                   m_stride.stride() == other.m_stride.stride() &&
                   m_step == other.m_step;
        }

        constexpr auto operator*() const noexcept -> reference
        {
            return m_stride;
        }

        constexpr auto operator+=(std::ptrdiff_t n) noexcept
                -> major_index_iterator&
        {
            m_stride = {
                    m_stride.data() + n * m_step,
                    m_stride.size(),
                    m_stride.stride()};
            return *this;
        }

        constexpr auto
        operator-(major_index_iterator const& other) const noexcept
        {
            BOOST_ASSERT(m_stride.size() == other.m_stride.size());
            BOOST_ASSERT(m_stride.stride() == other.m_stride.stride());
            BOOST_ASSERT(m_step == other.m_step);
            return (m_stride.data() - other.m_stride.data()) / m_step;
        }

    private:
        strided_span<U> m_stride;
        difference_type m_step{};
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

    [[nodiscard]] constexpr auto data() const noexcept -> T*
    {
        return m_data;
    }

    [[nodiscard]] constexpr auto major_size() const noexcept -> size_type
    {
        return m_major_size;
    }

    [[nodiscard]] constexpr auto minor_size() const noexcept -> size_type
    {
        return m_minor_size;
    }

    [[nodiscard]] constexpr auto major_step() const noexcept -> difference_type
    {
        return m_major_step;
    }

    [[nodiscard]] constexpr auto minor_step() const noexcept -> difference_type
    {
        return m_minor_step;
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator
    {
        return {m_data, m_major_step, m_minor_size, m_minor_step};
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return {m_data, m_major_step, m_minor_size, m_minor_step};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator
    {
        return {m_data + m_major_size * m_major_step,
                m_major_step,
                m_minor_size,
                m_minor_step};
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return {m_data + m_major_size * m_major_step,
                m_major_step,
                m_minor_size,
                m_minor_step};
    }

    [[nodiscard]] constexpr auto
    operator[](size_type major_index) const noexcept -> strided_span<T>
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
    BOOST_ASSERT(src.major_size() == dst.major_size());
    BOOST_ASSERT(src.minor_size() == dst.minor_size());

    for (auto src_it = src.begin(), src_end = src.end(), dst_it = dst.begin();
         src_it != src_end;
         ++src_it, ++dst_it)
    {
        std::ranges::transform(*src_it, dst_it->begin(), op);
    }
}

template <class T>
void
fill(table_view<T> const& tv, T const value)
{
    for (auto row : tv)
    {
        std::ranges::fill(row, value);
    }
}

template <class T>
auto
as_const(table_view<T> const& tv) -> table_view<std::add_const_t<T>>
{
    return {tv.data(),
            tv.major_size(),
            tv.minor_size(),
            tv.major_step(),
            tv.minor_step()};
}

} // namespace piejam::range
