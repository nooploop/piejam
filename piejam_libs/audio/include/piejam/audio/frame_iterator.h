// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <concepts>
#include <span>

namespace piejam::audio
{

template <std::size_t NumChannels>
constexpr std::size_t frame_extent =
        NumChannels == 0 ? std::dynamic_extent : NumChannels;

template <class T, std::size_t NumChannels = 0>
struct frame_iterator
    : boost::stl_interfaces::iterator_interface<
              frame_iterator<T, NumChannels>,
              std::random_access_iterator_tag,
              std::span<T, frame_extent<NumChannels>>>
{
    using value_type = std::span<T, frame_extent<NumChannels>>;
    using pointer = T*;

    constexpr frame_iterator() noexcept requires(NumChannels == 0) = default;
    constexpr frame_iterator() noexcept requires(NumChannels != 0)
        : m_frame(pointer{}, NumChannels)
    {
    }

    template <class U>
    constexpr frame_iterator(
            frame_iterator<U, NumChannels> const& other) noexcept
            requires(std::is_convertible_v<U*, T*>)
        : m_frame(other.m_frame)
    {
    }

    template <class U, std::size_t NumChannels2>
    constexpr frame_iterator(
            frame_iterator<U, NumChannels2> const& other) noexcept
            requires(std::is_convertible_v<U*, T*>&& NumChannels == 0)
        : m_frame(other.m_frame)
    {
    }

    template <class U>
    constexpr frame_iterator(frame_iterator<U, 0> const& other) noexcept
            requires(std::is_convertible_v<U*, T*>&& NumChannels != 0)
        : m_frame(other.m_frame)
    {
        BOOST_ASSERT(other.m_frame.size() == NumChannels);
    }

    constexpr frame_iterator(value_type const& frame) noexcept
        : m_frame(frame)
    {
    }

    constexpr auto num_channels() const noexcept -> std::size_t
    {
        return NumChannels == 0 ? m_frame.size() : NumChannels;
    }

    constexpr auto operator*() const noexcept -> value_type const&
    {
        return m_frame;
    }

    constexpr auto operator+=(std::ptrdiff_t const n) noexcept
            -> frame_iterator&
    {
        auto const frame_size = NumChannels == 0 ? m_frame.size() : NumChannels;
        BOOST_ASSERT(frame_size != 0);
        m_frame = value_type(m_frame.data() + n * frame_size, frame_size);
        return *this;
    }

    constexpr auto operator-(frame_iterator const& other) const noexcept
            -> std::ptrdiff_t
    {
        auto const frame_size = NumChannels == 0 ? m_frame.size() : NumChannels;
        BOOST_ASSERT(frame_size != 0);
        BOOST_ASSERT((m_frame.data() - other.m_frame.data()) % frame_size == 0);
        return (m_frame.data() - other.m_frame.data()) / frame_size;
    }

private:
    template <class U, std::size_t>
    friend struct frame_iterator;

    value_type m_frame;
};

} // namespace piejam::audio
