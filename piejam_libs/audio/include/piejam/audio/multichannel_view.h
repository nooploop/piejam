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

struct multichannel_layout_unspecified;
using multichannel_layout_interleaved = std::integral_constant<
        multichannel_layout,
        multichannel_layout::interleaved>;
using multichannel_layout_non_interleaved = std::integral_constant<
        multichannel_layout,
        multichannel_layout::non_interleaved>;

inline constexpr std::size_t dynamic_channels{};

template <
        class T,
        class Layout = multichannel_layout_unspecified,
        std::size_t NumChannels = dynamic_channels>
class multichannel_view
{
    using difference_type = typename range::table_view<T>::difference_type;

public:
    constexpr multichannel_view(
            std::span<T> data,
            multichannel_layout l,
            std::size_t num_channels) noexcept
        requires(std::is_same_v<Layout, multichannel_layout_unspecified> &&
                 NumChannels == dynamic_channels)
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
        requires(!std::is_same_v<Layout, multichannel_layout_unspecified> &&
                 NumChannels == dynamic_channels)
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
        requires(std::is_same_v<Layout, multichannel_layout_unspecified> &&
                 NumChannels != dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{l}
        , m_num_channels{NumChannels}
    {
        BOOST_ASSERT(m_data.size() % NumChannels == 0);
    }

    explicit constexpr multichannel_view(std::span<T> data) noexcept
        requires(!std::is_same_v<Layout, multichannel_layout_unspecified> &&
                 NumChannels != dynamic_channels)
        : m_data{std::move(data)}
        , m_layout{Layout::value}
        , m_num_channels{NumChannels}
    {
        BOOST_ASSERT(m_data.size() % NumChannels == 0);
    }

    [[nodiscard]] constexpr auto layout() const noexcept -> multichannel_layout
    {
        if constexpr (std::is_same_v<Layout, multichannel_layout_unspecified>)
        {
            return m_layout;
        }
        else
        {
            return Layout::value;
        }
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool
    {
        return m_data.empty();
    }

    [[nodiscard]] constexpr auto num_channels() const noexcept -> std::size_t
    {
        return NumChannels == 0 ? m_num_channels : NumChannels;
    }

    [[nodiscard]] constexpr auto num_frames() const noexcept -> std::size_t
    {
        return m_data.size() / num_channels();
    }

    [[nodiscard]] constexpr auto channels() const noexcept
    {
        if constexpr (std::is_same_v<Layout, multichannel_layout_unspecified>)
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

    [[nodiscard]] constexpr auto frames() const noexcept
    {
        if constexpr (std::is_same_v<Layout, multichannel_layout_unspecified>)
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

    [[nodiscard]] constexpr auto samples() const noexcept -> std::span<T> const&
    {
        return m_data;
    }

    template <class ToLayout, std::size_t ToNumChannels>
    [[nodiscard]] constexpr auto cast() const noexcept
    {
        if constexpr (std::is_same_v<ToLayout, multichannel_layout_unspecified>)
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

private:
    std::span<T> m_data;
    multichannel_layout m_layout{multichannel_layout::non_interleaved};
    std::size_t m_num_channels{};
};

template <class T>
multichannel_view(std::span<T>, multichannel_layout, std::size_t)
        -> multichannel_view<T>;

} // namespace piejam::audio
