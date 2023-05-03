// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_layout.h>

#include <piejam/range/table_view.h>

#include <boost/assert.hpp>

#include <span>

namespace piejam::audio
{

template <class T>
class multichannel_view
{
public:
    multichannel_view(
            multichannel_layout l,
            std::size_t num_channels,
            std::span<T> data)
        : m_layout{l}
        , m_num_channels{num_channels}
        , m_data{std::move(data)}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
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

    [[nodiscard]] auto channels() const noexcept
    {
        return channels(m_layout, m_num_channels, m_data);
    }

    [[nodiscard]] auto frames() const noexcept
    {
        return frames(m_layout, m_num_channels, m_data);
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
    template <class V>
    [[nodiscard]] static auto
    channels(multichannel_layout layout, std::size_t num_channels, V&& vec)
    {
        using difference_type = typename range::table_view<T>::difference_type;
        std::size_t const num_frames = vec.size() / num_channels;
        switch (layout)
        {
            case multichannel_layout::non_interleaved:
                return range::table_view{
                        vec.data(),
                        num_channels,
                        num_frames,
                        static_cast<difference_type>(num_frames),
                        1};

            case multichannel_layout::interleaved:
                return range::table_view{
                        vec.data(),
                        num_channels,
                        num_frames,
                        1,
                        static_cast<difference_type>(num_channels)};
        }
    }

    template <class V>
    [[nodiscard]] static auto
    frames(multichannel_layout layout, std::size_t num_channels, V&& vec)
    {
        using difference_type = typename range::table_view<T>::difference_type;
        std::size_t const num_frames = vec.size() / num_channels;
        switch (layout)
        {
            case multichannel_layout::non_interleaved:
                return range::table_view{
                        vec.data(),
                        num_frames,
                        num_channels,
                        1,
                        static_cast<difference_type>(num_frames)};

            case multichannel_layout::interleaved:
                return range::table_view{
                        vec.data(),
                        num_frames,
                        num_channels,
                        static_cast<difference_type>(num_channels),
                        1};
        }
    }

    multichannel_layout m_layout{multichannel_layout::non_interleaved};
    std::size_t m_num_channels{};
    std::span<T> m_data;
};

template <class T>
multichannel_view(multichannel_layout, std::size_t, std::span<T>)
        -> multichannel_view<T>;

} // namespace piejam::audio
