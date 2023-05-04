// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/stride_iterator.h>

#include <cassert>
#include <iterator>
#include <type_traits>

namespace piejam::range
{

template <class T>
class strided_span
{
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using iterator = stride_iterator<pointer>;
    using const_iterator = stride_iterator<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr strided_span() noexcept = default;

    constexpr strided_span(pointer ptr, size_type size, difference_type stride)
        : m_data(ptr)
        , m_size(size)
        , m_stride(stride)
    {
        assert((m_data && m_stride) || (!m_data && !m_size && !m_stride));
    }

    constexpr strided_span(strided_span const&) noexcept = default;

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator
    {
        return {m_data, m_stride};
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return {m_data, m_stride};
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator
    {
        return {m_data + m_size * m_stride, m_stride};
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return {m_data + m_size * m_stride, m_stride};
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto crbegin() const noexcept
            -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cend());
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto crend() const noexcept
            -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cbegin());
    }

    [[nodiscard]] constexpr auto front() const -> reference
    {
        assert(m_size);
        return *begin();
    }

    [[nodiscard]] constexpr auto back() const -> reference
    {
        assert(m_size);
        return *std::prev(end());
    }

    [[nodiscard]] constexpr auto data() const noexcept -> pointer
    {
        return m_data;
    }

    [[nodiscard]] constexpr auto operator[](size_type idx) const -> reference
    {
        assert(m_data && idx < m_size);
        return m_data[idx * m_stride];
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return m_size;
    }

    [[nodiscard]] constexpr auto stride() const noexcept -> difference_type
    {
        return m_stride;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return m_size == 0;
    }

private:
    pointer m_data{};
    size_type m_size{};
    difference_type m_stride{1};
};

} // namespace piejam::range
