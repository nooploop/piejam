// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/range/frame_iterator.h>
#include <piejam/range/interleaved_view.h>

#include <vector>

namespace piejam::range
{

template <class T, std::size_t NumChannels = 0>
class interleaved_vector
{
public:
    constexpr interleaved_vector() noexcept
            requires(NumChannels != 0) = default;

    constexpr interleaved_vector(std::size_t const num_channels) noexcept
        : m_num_channels(num_channels)
    {
        BOOST_ASSERT(interleaved_vector::num_channels() > 0);
    }

    constexpr interleaved_vector(std::vector<T> vec) requires(NumChannels != 0)
        : m_vec(std::move(vec))
    {
        BOOST_ASSERT(m_vec.size() % interleaved_vector::num_channels() == 0);
    }

    constexpr interleaved_vector(
            std::vector<T> vec,
            std::size_t const num_channels) requires(NumChannels == 0)
        : m_num_channels(num_channels)
        , m_vec(std::move(vec))
    {
        BOOST_ASSERT(m_vec.size() % interleaved_vector::num_channels() == 0);
    }

    constexpr auto num_channels() const noexcept -> std::size_t
    {
        return NumChannels == 0 ? m_num_channels : NumChannels;
    }

    constexpr bool empty() const noexcept { return m_vec.empty(); }

    constexpr auto begin() noexcept -> frame_iterator<T, NumChannels>
    {
        return std::span<T, frame_extent<NumChannels>>(
                m_vec.data(),
                num_channels());
    }

    constexpr auto end() noexcept -> frame_iterator<T, NumChannels>
    {
        BOOST_ASSERT(m_vec.size() % num_channels() == 0);
        return std::span<T, frame_extent<NumChannels>>(
                m_vec.data() + m_vec.size(),
                num_channels());
    }

    constexpr auto begin() const noexcept
            -> frame_iterator<T const, NumChannels>
    {
        return std::span<T const, frame_extent<NumChannels>>(
                m_vec.data(),
                num_channels());
    }

    constexpr auto end() const noexcept -> frame_iterator<T const, NumChannels>
    {
        BOOST_ASSERT(m_vec.size() % num_channels() == 0);
        return std::span<T const, frame_extent<NumChannels>>(
                m_vec.data() + m_vec.size(),
                num_channels());
    }

    constexpr auto frames() const noexcept
            -> interleaved_view<T const, NumChannels>
    {
        return {begin(), end()};
    }

    constexpr auto frames() noexcept -> interleaved_view<T, NumChannels>
    {
        return {begin(), end()};
    }

    constexpr auto data() const noexcept -> std::span<T const> { return m_vec; }
    constexpr auto data() noexcept -> std::span<T> { return m_vec; }

private:
    std::size_t m_num_channels{NumChannels};
    std::vector<T> m_vec;
};

} // namespace piejam::audio
