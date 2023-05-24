// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_view.h>

#include <mipp.h>

#include <boost/assert.hpp>

#include <span>
#include <vector>

namespace piejam::audio
{

template <class T, class Layout = multichannel_layout_runtime_defined>
class multichannel_buffer
{
    static inline constexpr auto default_layout = std::conditional_t<
            std::is_same_v<Layout, multichannel_layout_runtime_defined>,
            multichannel_layout_non_interleaved,
            Layout>::value;

public:
    using vector = mipp::vector<T>;

    multichannel_buffer() noexcept = default;

    explicit multichannel_buffer(std::size_t num_channels)
        : m_num_channels{num_channels}
    {
        BOOST_ASSERT(num_channels > 0);
    }

    multichannel_buffer(std::size_t num_channels, std::size_t num_frames)
        : m_num_channels{num_channels}
        , m_data(num_channels * num_frames)
    {
        BOOST_ASSERT(num_channels > 0);
    }

    multichannel_buffer(
            multichannel_layout layout,
            std::size_t num_channels,
            std::size_t num_frames)
        requires(std::is_same_v<Layout, multichannel_layout_runtime_defined>)
        : m_layout{layout}
        , m_num_channels{num_channels}
        , m_data(num_channels * num_frames)
    {
        BOOST_ASSERT(num_channels > 0);
    }

    multichannel_buffer(
            multichannel_layout l,
            std::size_t num_channels,
            vector data)
        requires(std::is_same_v<Layout, multichannel_layout_runtime_defined>)
        : m_layout{l}
        , m_num_channels{num_channels}
        , m_data{std::move(data)}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
    }

    multichannel_buffer(std::size_t num_channels, vector data)
        requires(!std::is_same_v<Layout, multichannel_layout_runtime_defined>)
        : m_num_channels{num_channels}
        , m_data{std::move(data)}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
    }

    [[nodiscard]] auto view() noexcept
    {
        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            return multichannel_view<T, Layout>{
                    std::span{m_data},
                    m_layout,
                    m_num_channels};
        }
        else
        {
            return multichannel_view<T, Layout>{
                    std::span{m_data},
                    m_num_channels};
        }
    }

    [[nodiscard]] auto view() const noexcept
    {
        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            return multichannel_view<std::add_const_t<T>, Layout>{
                    std::span{m_data},
                    m_layout,
                    m_num_channels};
        }
        else
        {
            return multichannel_view<std::add_const_t<T>, Layout>{
                    std::span{m_data},
                    m_num_channels};
        }
    }

    [[nodiscard]] constexpr auto layout() const noexcept -> multichannel_layout
    {
        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            return m_layout;
        }
        else
        {
            return Layout::value;
        }
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_data.empty();
    }

    [[nodiscard]] constexpr auto num_channels() const noexcept -> std::size_t
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

    [[nodiscard]] auto samples() noexcept -> std::span<T>
    {
        return m_data;
    }

    [[nodiscard]] auto samples() const noexcept -> std::span<T const>
    {
        return m_data;
    }

private:
    multichannel_layout m_layout{default_layout};
    std::size_t m_num_channels{1};
    vector m_data;
};

} // namespace piejam::audio
