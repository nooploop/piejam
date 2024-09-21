// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/stride_pointer_iterator.h>

#include <boost/assert.hpp>

#include <iterator>
#include <type_traits>

namespace piejam::range
{

template <class T, std::ptrdiff_t Stride = dynamic_stride>
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
    using iterator = stride_pointer_iterator<T, Stride>;
    using const_iterator = stride_pointer_iterator<T const, Stride>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr strided_span() noexcept = default;

    constexpr strided_span(
            pointer ptr,
            size_type size,
            difference_type stride) noexcept
        requires(Stride == dynamic_stride)
        : m_data(ptr)
        , m_size(size)
        , m_stride(stride)
    {
        BOOST_ASSERT(m_stride > 0);
    }

    constexpr strided_span(pointer ptr, size_type size) noexcept
        requires(Stride != dynamic_stride)
        : m_data(ptr)
        , m_size(size)
    {
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> iterator
    {
        return make_iterator<iterator>(m_data, stride());
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator
    {
        return make_iterator<const_iterator>(m_data, stride());
    }

    [[nodiscard]] constexpr auto end() const noexcept -> iterator
    {
        return make_iterator<iterator>(m_data + m_size * stride(), stride());
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator
    {
        return make_iterator<const_iterator>(
                m_data + m_size * stride(),
                stride());
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(end());
    }

    [[nodiscard]] constexpr auto
    crbegin() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cend());
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr auto
    crend() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cbegin());
    }

    [[nodiscard]] constexpr auto front() const -> reference
    {
        BOOST_ASSERT(m_size);
        return *begin();
    }

    [[nodiscard]] constexpr auto back() const -> reference
    {
        BOOST_ASSERT(m_size);
        return *std::prev(end());
    }

    [[nodiscard]] constexpr auto data() const noexcept -> pointer
    {
        return m_data;
    }

    [[nodiscard]] constexpr auto operator[](size_type idx) const -> reference
    {
        BOOST_ASSERT(m_data && idx < m_size);
        return m_data[idx * stride()];
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type
    {
        return m_size;
    }

    [[nodiscard]] constexpr auto stride() const noexcept -> difference_type
    {
        return Stride == dynamic_stride ? m_stride : Stride;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return m_size == 0;
    }

    template <difference_type ToStride>
    [[nodiscard]] constexpr auto
    stride_cast() const noexcept -> strided_span<T, ToStride>
        requires(Stride == dynamic_stride && ToStride != dynamic_stride)
    {
        BOOST_ASSERT(m_stride == ToStride);
        return {m_data, m_size};
    }

private:
    template <class Iterator, class Data>
    static constexpr auto
    make_iterator(Data&& d, difference_type stride) -> Iterator
    {
        if constexpr (Stride == dynamic_stride)
        {
            return Iterator{d, stride};
        }
        else
        {
            return Iterator{d};
        }
    }

    pointer m_data{};
    size_type m_size{};
    difference_type m_stride{Stride == dynamic_stride ? 1 : Stride};
};

} // namespace piejam::range
