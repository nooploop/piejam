// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/interleaved_view.h>

#include <piejam/range/span_iterator.h>

#include <vector>

namespace piejam::audio
{

template <class T, std::size_t NumChannels = std::dynamic_extent>
class interleaved_vector
{
public:
    constexpr interleaved_vector() noexcept = default;

    constexpr interleaved_vector(std::size_t const num_channels) noexcept
        : m_num_channels(num_channels)
    {
        BOOST_ASSERT(interleaved_vector::num_channels() > 0);
    }

    constexpr interleaved_vector(std::vector<T> vec)
        requires(NumChannels != std::dynamic_extent)
        : m_vec(std::move(vec))
    {
        BOOST_ASSERT(m_vec.size() % interleaved_vector::num_channels() == 0);
    }

    constexpr interleaved_vector(
            std::vector<T> vec,
            std::size_t const num_channels)
        requires(NumChannels == std::dynamic_extent)
        : m_num_channels(num_channels)
        , m_vec(std::move(vec))
    {
        BOOST_ASSERT(m_vec.size() % interleaved_vector::num_channels() == 0);
    }

    [[nodiscard]] constexpr auto num_channels() const noexcept -> std::size_t
    {
        return NumChannels == std::dynamic_extent ? m_num_channels
                                                  : NumChannels;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return m_vec.empty();
    }

    [[nodiscard]] constexpr auto begin() noexcept
            -> range::span_iterator<T, NumChannels>
    {
        return std::span<T, NumChannels>(m_vec.data(), num_channels());
    }

    [[nodiscard]] constexpr auto end() noexcept
            -> range::span_iterator<T, NumChannels>
    {
        BOOST_ASSERT(num_channels() != 0);
        BOOST_ASSERT(m_vec.size() % num_channels() == 0);
        return std::span<T, NumChannels>(
                m_vec.data() + m_vec.size(),
                num_channels());
    }

    [[nodiscard]] constexpr auto begin() const noexcept
            -> range::span_iterator<T const, NumChannels>
    {
        return std::span<T const, NumChannels>(m_vec.data(), num_channels());
    }

    [[nodiscard]] constexpr auto end() const noexcept
            -> range::span_iterator<T const, NumChannels>
    {
        BOOST_ASSERT(num_channels() != 0);
        BOOST_ASSERT(m_vec.size() % num_channels() == 0);
        return std::span<T const, NumChannels>(
                m_vec.data() + m_vec.size(),
                num_channels());
    }

    [[nodiscard]] constexpr auto frames() const noexcept
            -> interleaved_view<T const, NumChannels>
    {
        return {begin(), end()};
    }

    [[nodiscard]] constexpr auto frames() noexcept
            -> interleaved_view<T, NumChannels>
    {
        return {begin(), end()};
    }

    [[nodiscard]] constexpr auto num_frames() const noexcept -> std::size_t
    {
        return m_vec.size() / m_num_channels;
    }

    [[nodiscard]] constexpr auto samples() const noexcept -> std::span<T const>
    {
        return m_vec;
    }

    [[nodiscard]] constexpr auto samples() noexcept -> std::span<T>
    {
        return m_vec;
    }

private:
    std::size_t m_num_channels{NumChannels};
    std::vector<T> m_vec;
};

} // namespace piejam::audio
