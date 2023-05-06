// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <iterator>
#include <type_traits>

namespace piejam::range
{

namespace detail
{

template <class Iterator, std::iter_difference_t<Iterator> Stride>
using stride_iterator_category = std::conditional_t<
        std::random_access_iterator<Iterator> && Stride == 1,
        std::contiguous_iterator_tag,
        typename std::iterator_traits<Iterator>::iterator_category>;

} // namespace detail

inline constexpr std::ptrdiff_t dynamic_stride{};

template <
        class Iterator,
        std::iter_difference_t<Iterator> Stride = dynamic_stride>
class stride_iterator
    : public boost::stl_interfaces::iterator_interface<
              stride_iterator<Iterator, Stride>,
              detail::stride_iterator_category<Iterator, Stride>,
              typename std::iterator_traits<Iterator>::value_type,
              typename std::iterator_traits<Iterator>::reference,
              typename std::iterator_traits<Iterator>::pointer,
              typename std::iterator_traits<Iterator>::difference_type>
{
    using base_type = boost::stl_interfaces::iterator_interface<
            stride_iterator<Iterator, Stride>,
            detail::stride_iterator_category<Iterator, Stride>,
            typename std::iterator_traits<Iterator>::value_type,
            typename std::iterator_traits<Iterator>::reference,
            typename std::iterator_traits<Iterator>::pointer,
            typename std::iterator_traits<Iterator>::difference_type>;

public:
    using iterator_category = typename base_type::iterator_category;

    // clang-format off

    constexpr stride_iterator()
            noexcept(std::is_nothrow_default_constructible_v<Iterator>) = default;

    constexpr stride_iterator(
            Iterator const& it,
            typename base_type::difference_type stride)
            noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        requires (Stride == dynamic_stride)
        : m_it{it}
        , m_stride{stride}
    {
        BOOST_ASSERT(m_stride > dynamic_stride);
    }

    explicit constexpr stride_iterator(
            Iterator const& it)
            noexcept(std::is_nothrow_copy_constructible_v<Iterator>)
        requires (Stride != dynamic_stride)
        : m_it{it}
    {
    }

    constexpr stride_iterator(
            Iterator&& it,
            typename base_type::difference_type stride)
            noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        requires (Stride == dynamic_stride)
        : m_it{std::move(it)}
        , m_stride{stride}
    {
        BOOST_ASSERT(m_stride > 0);
    }

    explicit constexpr stride_iterator(
            Iterator&& it)
            noexcept(std::is_nothrow_move_constructible_v<Iterator>)
        requires (Stride != dynamic_stride)
        : m_it{std::move(it)}
    {
        BOOST_ASSERT(m_stride > 0);
    }

    // clang-format on

    [[nodiscard]] constexpr auto stride() const noexcept ->
            typename base_type::difference_type
    {
        return Stride == dynamic_stride ? m_stride : Stride;
    }

    auto operator++() -> stride_iterator&
        requires requires(Iterator it) { ++it; }
    {
        std::advance(m_it, stride());
        return *this;
    }

    using base_type::operator++;

    auto operator--() -> stride_iterator&
        requires requires(Iterator it) { --it; }
    {
        std::advance(m_it, -stride());
        return *this;
    }

    using base_type::operator--;

    [[nodiscard]] constexpr auto operator*() noexcept -> decltype(auto)
    {
        return *m_it;
    }

    [[nodiscard]] constexpr auto operator*() const noexcept -> decltype(auto)
    {
        return *m_it;
    }

    constexpr auto operator+=(typename base_type::difference_type n)
            -> stride_iterator&
        requires requires(Iterator it) { it + n; }
    {
        std::advance(m_it, n * stride());
        return *this;
    }

    [[nodiscard]] friend constexpr auto
    operator==(stride_iterator const& lhs, stride_iterator const& rhs) noexcept
            -> bool
    {
        return lhs.m_it == rhs.m_it && lhs.stride() == rhs.stride();
    }

    [[nodiscard]] constexpr auto
    operator-(stride_iterator const& rhs) const noexcept ->
            typename base_type::difference_type
        requires requires(
                Iterator l,
                Iterator r,
                typename base_type::difference_type s) { (l - r) / s; }
    {
        BOOST_ASSERT(stride() == rhs.stride());
        return (m_it - rhs.m_it) / m_stride;
    }

private:
    Iterator m_it{};
    typename base_type::difference_type m_stride{
            Stride == dynamic_stride ? 1 : Stride};
};

template <class Iterator>
stride_iterator(
        Iterator const&,
        typename std::iterator_traits<Iterator>::difference_type)
        -> stride_iterator<Iterator>;

template <class Iterator>
stride_iterator(
        Iterator&&,
        typename std::iterator_traits<Iterator>::difference_type)
        -> stride_iterator<Iterator>;

} // namespace piejam::range
