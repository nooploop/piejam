// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_view.h>

#include <piejam/range/table_view.h>

#include <boost/assert.hpp>

#include <span>
#include <vector>

namespace piejam::audio
{

template <class T>
class multichannel_buffer
{
public:
    explicit multichannel_buffer(std::size_t num_channels)
        : multichannel_buffer{
                  multichannel_layout::non_interleaved,
                  num_channels,
                  0}
    {
    }

    multichannel_buffer(std::size_t num_channels, std::size_t num_frames)
        : multichannel_buffer{
                  multichannel_layout::non_interleaved,
                  num_channels,
                  num_frames}
    {
    }

    multichannel_buffer(
            multichannel_layout layout,
            std::size_t num_channels,
            std::size_t num_frames)
        : m_layout{layout}
        , m_num_channels{num_channels}
        , m_data(num_channels * num_frames, T{})
    {
        BOOST_ASSERT(num_channels > 0);
    }

    multichannel_buffer(
            multichannel_layout l,
            std::size_t num_channels,
            std::vector<T> data)
        : m_layout{l}
        , m_num_channels{num_channels}
        , m_data{std::move(data)}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
    }

    [[nodiscard]] auto view() noexcept
    {
        return multichannel_view{m_layout, m_num_channels, std::span{m_data}};
    }

    [[nodiscard]] auto view() const noexcept
    {
        return multichannel_view{m_layout, m_num_channels, std::span{m_data}};
    }

    [[nodiscard]] auto layout() const noexcept -> multichannel_layout
    {
        return m_layout;
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_data.empty();
    }

    [[nodiscard]] auto num_channels() const noexcept -> std::size_t
    {
        return m_num_channels;
    }

    [[nodiscard]] auto num_frames() const noexcept -> std::size_t
    {
        return m_data.size() / m_num_channels;
    }

    [[nodiscard]] auto channels() noexcept
    {
        return view().channels();
    }

    [[nodiscard]] auto channels() const noexcept
    {
        return view().channels();
    }

    [[nodiscard]] auto frames() noexcept
    {
        return view().frames();
    }

    [[nodiscard]] auto frames() const noexcept
    {
        return view().frames();
    }

    [[nodiscard]] auto samples() noexcept
    {
        return m_data;
    }

    [[nodiscard]] auto samples() const noexcept
    {
        return m_data;
    }

private:
    multichannel_layout m_layout{multichannel_layout::non_interleaved};
    std::size_t m_num_channels{};
    std::vector<T> m_data;
};

} // namespace piejam::audio
