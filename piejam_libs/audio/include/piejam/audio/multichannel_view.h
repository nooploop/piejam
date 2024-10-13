// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_layout.h>

#include <piejam/range/table_view.h>

#include <boost/assert.hpp>

#include <span>

namespace piejam::audio
{

struct multichannel_layout_runtime_defined;
using multichannel_layout_interleaved = std::integral_constant<
        multichannel_layout,
        multichannel_layout::interleaved>;
using multichannel_layout_non_interleaved = std::integral_constant<
        multichannel_layout,
        multichannel_layout::non_interleaved>;

inline constexpr std::size_t dynamic_channels{};

template <
        class T,
        class Layout = multichannel_layout_runtime_defined,
        std::size_t NumChannels = dynamic_channels>
class multichannel_view
{
    using difference_type = typename range::table_view<T>::difference_type;

    static constexpr bool layout_is_runtime_defined =
            std::is_same_v<Layout, multichannel_layout_runtime_defined>;

public:
    constexpr multichannel_view(
            std::span<T> data,
            multichannel_layout l,
            std::size_t num_channels) noexcept
        requires(layout_is_runtime_defined && NumChannels == dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{l}
        , m_num_channels{num_channels}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
    }

    constexpr multichannel_view(
            std::span<T> data,
            std::size_t num_channels) noexcept
        requires(!layout_is_runtime_defined && NumChannels == dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{Layout::value}
        , m_num_channels{num_channels}
    {
        BOOST_ASSERT(num_channels > 0);
        BOOST_ASSERT(m_data.size() % num_channels == 0);
    }

    constexpr multichannel_view(
            std::span<T> data,
            multichannel_layout l) noexcept
        requires(layout_is_runtime_defined && NumChannels != dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{l}
        , m_num_channels{NumChannels}
    {
        BOOST_ASSERT(m_data.size() % NumChannels == 0);
    }

    explicit constexpr multichannel_view(std::span<T> data) noexcept
        requires(!layout_is_runtime_defined && NumChannels != dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{Layout::value}
        , m_num_channels{NumChannels}
    {
        BOOST_ASSERT(m_data.size() % NumChannels == 0);
    }

    [[nodiscard]]
    constexpr auto layout() const noexcept -> multichannel_layout
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

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool
    {
        return m_data.empty();
    }

    [[nodiscard]]
    constexpr auto num_channels() const noexcept -> std::size_t
    {
        return NumChannels == 0 ? m_num_channels : NumChannels;
    }

    [[nodiscard]]
    constexpr auto num_frames() const noexcept -> std::size_t
    {
        return m_data.size() / num_channels();
    }

    [[nodiscard]]
    constexpr auto channels() const noexcept
    {
        if constexpr (layout_is_runtime_defined)
        {
            switch (layout())
            {
                case multichannel_layout::non_interleaved:
                    return range::table_view{
                            m_data.data(),
                            num_channels(),
                            num_frames(),
                            static_cast<difference_type>(num_frames()),
                            1};

                case multichannel_layout::interleaved:
                    return range::table_view{
                            m_data.data(),
                            num_channels(),
                            num_frames(),
                            1,
                            static_cast<difference_type>(num_channels())};
            }
        }
        else if constexpr (std::is_same_v<
                                   Layout,
                                   multichannel_layout_non_interleaved>)
        {
            if constexpr (NumChannels == dynamic_channels)
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        std::dynamic_extent,
                        range::dynamic_stride,
                        1>{
                        m_data.data(),
                        num_channels(),
                        num_frames(),
                        static_cast<difference_type>(num_frames()),
                        1};
            }
            else
            {
                return range::table_view<
                        T,
                        NumChannels,
                        std::dynamic_extent,
                        range::dynamic_stride,
                        1>{
                        m_data.data(),
                        num_channels(),
                        num_frames(),
                        static_cast<difference_type>(num_frames()),
                        1};
            }
        }
        else if constexpr (std::is_same_v<
                                   Layout,
                                   multichannel_layout_interleaved>)
        {
            if constexpr (NumChannels == dynamic_channels)
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        std::dynamic_extent,
                        1,
                        range::dynamic_stride>{
                        m_data.data(),
                        num_channels(),
                        num_frames(),
                        1,
                        static_cast<difference_type>(num_channels())};
            }
            else
            {
                return range::table_view<
                        T,
                        NumChannels,
                        std::dynamic_extent,
                        1,
                        NumChannels>{
                        m_data.data(),
                        num_channels(),
                        num_frames(),
                        1,
                        static_cast<difference_type>(num_channels())};
            }
        }
    }

    [[nodiscard]]
    constexpr auto channels_subview(
            std::size_t start_channel,
            std::size_t num_channels_subview) const noexcept
        requires(!std::is_same_v<Layout, multichannel_layout_interleaved>)
    {
        BOOST_ASSERT(num_channels_subview > 0);
        BOOST_ASSERT(start_channel + num_channels_subview <= num_channels());
        BOOST_ASSERT(layout() == multichannel_layout::non_interleaved);

        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            return multichannel_view<T>{
                    std::span{
                            channels()[start_channel].data(),
                            num_channels_subview * num_frames()},
                    layout(),
                    num_channels_subview};
        }
        else
        {
            return multichannel_view<T, Layout>{
                    std::span{
                            channels()[start_channel].data(),
                            num_channels_subview * num_frames()},
                    num_channels_subview};
        }
    }

    [[nodiscard]]
    constexpr auto frames() const noexcept
    {
        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            switch (layout())
            {
                case multichannel_layout::non_interleaved:
                    return range::table_view{
                            m_data.data(),
                            num_frames(),
                            num_channels(),
                            1,
                            static_cast<difference_type>(num_frames())};

                case multichannel_layout::interleaved:
                    return range::table_view{
                            m_data.data(),
                            num_frames(),
                            num_channels(),
                            static_cast<difference_type>(num_channels()),
                            1};
            }
        }
        else if constexpr (std::is_same_v<
                                   Layout,
                                   multichannel_layout_non_interleaved>)
        {
            if constexpr (NumChannels == dynamic_channels)
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        std::dynamic_extent,
                        1,
                        range::dynamic_stride>{
                        m_data.data(),
                        num_frames(),
                        num_channels(),
                        1,
                        static_cast<difference_type>(num_frames())};
            }
            else
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        NumChannels,
                        1,
                        range::dynamic_stride>{
                        m_data.data(),
                        num_frames(),
                        num_channels(),
                        1,
                        static_cast<difference_type>(num_frames())};
            }
        }
        else if constexpr (std::is_same_v<
                                   Layout,
                                   multichannel_layout_interleaved>)
        {
            if constexpr (NumChannels == dynamic_channels)
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        std::dynamic_extent,
                        range::dynamic_stride,
                        1>{
                        m_data.data(),
                        num_frames(),
                        num_channels(),
                        static_cast<difference_type>(num_channels()),
                        1};
            }
            else
            {
                return range::table_view<
                        T,
                        std::dynamic_extent,
                        NumChannels,
                        NumChannels,
                        1>{
                        m_data.data(),
                        num_frames(),
                        num_channels(),
                        static_cast<difference_type>(num_channels()),
                        1};
            }
        }
    }

    [[nodiscard]]
    constexpr auto frames_subview(
            std::size_t start_frame,
            std::size_t num_frames_subview) const noexcept
        requires(!std::is_same_v<Layout, multichannel_layout_non_interleaved>)
    {
        BOOST_ASSERT(num_frames_subview > 0);
        BOOST_ASSERT(start_frame + num_frames_subview <= num_frames());
        BOOST_ASSERT(layout() == multichannel_layout::interleaved);

        if constexpr (std::is_same_v<
                              Layout,
                              multichannel_layout_runtime_defined>)
        {
            return multichannel_view<T>{
                    std::span{
                            frames()[start_frame].data(),
                            num_frames_subview * num_channels()},
                    layout(),
                    num_channels()};
        }
        else
        {
            return multichannel_view<T, Layout>{
                    std::span{
                            channels()[start_frame].data(),
                            num_frames_subview * num_channels()},
                    num_channels()};
        }
    }

    [[nodiscard]]
    constexpr auto samples() const noexcept -> std::span<T>
    {
        return m_data;
    }

    template <class ToLayout, std::size_t ToNumChannels>
    [[nodiscard]]
    constexpr auto cast() const noexcept
    {
        if constexpr (std::is_same_v<
                              ToLayout,
                              multichannel_layout_runtime_defined>)
        {
            if constexpr (ToNumChannels == dynamic_channels)
            {
                return multichannel_view<T, ToLayout, ToNumChannels>{
                        m_data,
                        layout(),
                        num_channels()};
            }
            else
            {
                BOOST_ASSERT(num_channels() == ToNumChannels);
                return multichannel_view<T, ToLayout, ToNumChannels>(
                        m_data,
                        layout());
            }
        }
        else
        {
            BOOST_ASSERT(layout() == ToLayout::value);
            if constexpr (ToNumChannels == dynamic_channels)
            {
                return multichannel_view<T, ToLayout, ToNumChannels>{
                        m_data,
                        num_channels()};
            }
            else
            {
                BOOST_ASSERT(num_channels() == ToNumChannels);
                return multichannel_view<T, ToLayout, ToNumChannels>{m_data};
            }
        }
    }

    template <std::size_t ToNumChannels>
        requires(!layout_is_runtime_defined)
    [[nodiscard]]
    constexpr auto channels_cast() const noexcept
    {
        return cast<Layout, ToNumChannels>();
    }

private:
    std::span<T> m_data;
    multichannel_layout m_layout{multichannel_layout::non_interleaved};
    std::size_t m_num_channels{};
};

template <class T>
multichannel_view(std::span<T>, multichannel_layout, std::size_t)
        -> multichannel_view<T>;

} // namespace piejam::audio
