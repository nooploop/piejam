// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/span_iterator.h>

#include <ranges>

namespace piejam::audio
{

template <class T, std::size_t NumChannels = std::dynamic_extent>
struct interleaved_view
    : std::ranges::view_interface<interleaved_view<T, NumChannels>>
{
    using iterator = range::span_iterator<T, NumChannels>;

    constexpr interleaved_view() noexcept = default;

    constexpr interleaved_view(iterator first, iterator last) noexcept
        : m_begin(std::move(first))
        , m_end(std::move(last))
    {
        BOOST_ASSERT(m_begin->template size() == m_end->template size());
    }

    constexpr interleaved_view(
            T* const data,
            std::size_t const num_channels,
            std::size_t const num_frames) noexcept
        requires(NumChannels == std::dynamic_extent)
        : m_begin(std::span<T, NumChannels>(data, num_channels))
        , m_end(m_begin + num_frames)
    {
    }

    constexpr interleaved_view(
            T* const data,
            std::size_t const num_frames) noexcept
        requires(NumChannels != std::dynamic_extent)
        : m_begin(std::span<T, NumChannels>(data, NumChannels))
        , m_end(m_begin + num_frames)
    {
    }

    template <class U>
    constexpr interleaved_view(interleaved_view<U, NumChannels> const& other)
        requires(std::is_convertible_v<U*, T*>)
        : m_begin(other.begin())
        , m_end(other.end())
    {
    }

    [[nodiscard]] constexpr auto begin() const -> iterator
    {
        return m_begin;
    }

    [[nodiscard]] constexpr auto end() const -> iterator
    {
        return m_end;
    }

    [[nodiscard]] constexpr auto num_channels() const noexcept -> std::size_t
        requires(NumChannels == std::dynamic_extent)
    {
        return m_begin->template size();
    }

    [[nodiscard]] constexpr auto num_channels() const noexcept -> std::size_t
        requires(NumChannels != std::dynamic_extent)
    {
        return NumChannels;
    }

    [[nodiscard]] constexpr auto num_frames() const noexcept -> std::size_t
    {
        return std::distance(m_begin, m_end);
    }

    template <std::size_t ToNumChannels>
    [[nodiscard]] constexpr auto channels_cast() const noexcept
            -> interleaved_view<T, ToNumChannels>
        requires(NumChannels == std::dynamic_extent)
    {
        BOOST_ASSERT(m_begin->template size() == ToNumChannels);
        return interleaved_view<T, ToNumChannels>(m_begin, m_end);
    }

private:
    iterator m_begin;
    iterator m_end;
};

} // namespace piejam::audio
