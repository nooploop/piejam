// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/dynamic_stride.h>

#include <boost/assert.hpp>

#include <iterator>
#include <type_traits>

namespace piejam::range
{

template <typename T, std::ptrdiff_t Stride = dynamic_stride>
class pointer_stride_iterator
{
public:
    using iterator_concept = std::conditional_t<
            Stride == 1,
            std::contiguous_iterator_tag,
            std::random_access_iterator_tag>;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;

    constexpr pointer_stride_iterator() noexcept = default;

    explicit constexpr pointer_stride_iterator(pointer ptr) noexcept
        requires(Stride != dynamic_stride)
        : m_p(ptr)
    {
    }

    constexpr pointer_stride_iterator(
            pointer ptr,
            difference_type stride) noexcept
        requires(Stride == dynamic_stride)
        : m_p(ptr)
        , m_stride(stride)
    {
        BOOST_ASSERT(stride != 0);
    }

    [[nodiscard]]
    constexpr auto stride() const noexcept -> difference_type
    {
        return Stride == dynamic_stride ? m_stride : Stride;
    }

    [[nodiscard]]
    constexpr auto operator*() const noexcept -> reference
    {
        return *m_p;
    }

    [[nodiscard]]
    constexpr auto operator->() const noexcept -> pointer
    {
        return m_p;
    }

    [[nodiscard]]
    constexpr auto operator[](difference_type n) const noexcept -> reference
    {
        return *(m_p + n * stride());
    }

    constexpr auto operator++() noexcept -> pointer_stride_iterator&
    {
        m_p += stride();
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator++(int) noexcept -> pointer_stride_iterator
    {
        pointer_stride_iterator temp(*this);
        ++(*this);
        return temp;
    }

    constexpr auto operator--() noexcept -> pointer_stride_iterator&
    {
        m_p -= stride();
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator--(int) noexcept -> pointer_stride_iterator
    {
        pointer_stride_iterator temp(*this);
        --(*this);
        return temp;
    }

    constexpr auto
    operator+=(difference_type n) noexcept -> pointer_stride_iterator&
    {
        m_p += n * stride();
        return *this;
    }

    [[nodiscard]]
    constexpr auto
    operator+(difference_type n) const noexcept -> pointer_stride_iterator
    {
        pointer_stride_iterator temp(*this);
        return temp += n;
    }

    [[nodiscard]]
    friend constexpr auto
    operator+(difference_type n, pointer_stride_iterator const& it) noexcept
            -> pointer_stride_iterator
    {
        return it + n;
    }

    constexpr auto
    operator-=(difference_type n) noexcept -> pointer_stride_iterator&
    {
        return *this += -n;
    }

    [[nodiscard]]
    constexpr auto
    operator-(difference_type n) const noexcept -> pointer_stride_iterator
    {
        pointer_stride_iterator temp(*this);
        return temp -= n;
    }

    [[nodiscard]]
    constexpr auto operator-(pointer_stride_iterator const& rhs) const noexcept
            -> difference_type
    {
        verify_precondition(rhs);
        return (m_p - rhs.m_p) / stride();
    }

    [[nodiscard]]
    constexpr auto
    operator==(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        verify_precondition(rhs);
        return m_p == rhs.m_p;
    }

    [[nodiscard]]
    constexpr auto
    operator!=(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        return !(*this == rhs);
    }

    [[nodiscard]]
    constexpr auto
    operator<(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        verify_precondition(rhs);
        return m_p < rhs.m_p;
    }

    [[nodiscard]]
    constexpr auto
    operator>(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        return rhs < *this;
    }

    [[nodiscard]]
    constexpr auto
    operator<=(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        return !(rhs < *this);
    }

    [[nodiscard]]
    constexpr auto
    operator>=(pointer_stride_iterator const& rhs) const noexcept -> bool
    {
        return !(*this < rhs);
    }

private:
    void verify_precondition(
            [[maybe_unused]] pointer_stride_iterator const& rhs) const
    {
        BOOST_ASSERT(stride() == rhs.stride());
        BOOST_ASSERT((m_p - rhs.m_p) % stride() == 0);
    }

    pointer m_p{};
    difference_type m_stride{Stride == dynamic_stride ? 1 : Stride};
};

template <class T>
pointer_stride_iterator(T*, std::ptrdiff_t) -> pointer_stride_iterator<T>;

} // namespace piejam::range
