// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/audio/slice_algorithms.h>

#include <piejam/thread/cache_line_size.h>

#include <boost/assert.hpp>

#include <atomic>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

//! single-producer-single-consumer, lock-free, multi-channel ring-buffer.
//! writes from rt-thread, reads from main.
template <class T>
class stream_ring_buffer
{
public:
    using multichannel_buffer_t =
            multichannel_buffer<T, multichannel_layout_non_interleaved>;

    stream_ring_buffer(
            std::size_t const num_channels,
            std::size_t const capacity_per_channel)
        : m_num_channels{num_channels}
        , m_capacity_per_channel{capacity_per_channel + 1}
        , m_buffer(m_num_channels * m_capacity_per_channel)
    {
    }

    //! returns frames written
    auto
    write(std::span<std::reference_wrapper<slice<T> const> const> const data,
          std::size_t size_per_channel) -> std::size_t
    {
        BOOST_ASSERT(data.size() == m_num_channels);

        std::size_t const write_index =
                m_write_index.load(std::memory_order_acquire);
        std::size_t const read_index = m_read_index.load();

        std::size_t const available = write_available(
                write_index,
                read_index,
                m_capacity_per_channel);

        if (available == 0)
        {
            return 0;
        }

        std::size_t const write_size = std::min(size_per_channel, available);
        BOOST_ASSERT(write_size > 0);

        std::size_t new_write_index = write_index + write_size;

        if (new_write_index > m_capacity_per_channel)
        {
            std::size_t const middle = m_capacity_per_channel - write_index;
            std::size_t rest_size = write_size - middle;

            for (std::size_t ch = 0; ch < m_num_channels; ++ch)
            {
                slice<T> src_data = data[ch];
                auto dst_data = m_buffer_multi_channel_view[ch];

                copy(subslice(src_data, 0, middle),
                     std::span<float>{dst_data.data() + write_index, middle});
                copy(subslice(src_data, middle, rest_size),
                     std::span<float>{dst_data.data(), rest_size});
            }

            new_write_index -= m_capacity_per_channel;
        }
        else
        {
            for (std::size_t ch = 0; ch < m_num_channels; ++ch)
            {
                slice<T> src_data = data[ch];
                auto dst_data = m_buffer_multi_channel_view[ch];

                copy(subslice(src_data, 0, write_size),
                     std::span<float>{
                             dst_data.data() + write_index,
                             write_size});
            }

            if (new_write_index == m_capacity_per_channel)
            {
                new_write_index = 0;
            }
        }

        m_write_index.store(new_write_index, std::memory_order_release);
        return write_size;
    }

    auto consume() -> multichannel_buffer_t
    {
        std::size_t const read_index =
                m_read_index.load(std::memory_order_acquire);
        std::size_t const write_index = m_write_index.load();

        if (write_index == read_index)
        {
            return multichannel_buffer_t{m_num_channels};
        }

        multichannel_buffer<float>::vector result;
        std::size_t frames_to_consume;

        if (write_index < read_index)
        {
            frames_to_consume =
                    m_capacity_per_channel - read_index + write_index;
            result.reserve(frames_to_consume * m_num_channels);

            for (auto d = m_buffer.data(), e = d + m_buffer.size(); d < e;
                 d += m_capacity_per_channel)
            {
                result.insert(
                        result.end(),
                        d + read_index,
                        d + m_capacity_per_channel);

                result.insert(result.end(), d, d + write_index);
            }
        }
        else
        {
            frames_to_consume = write_index - read_index;
            result.reserve(frames_to_consume * m_num_channels);

            for (auto d = m_buffer.data() + read_index, e = d + m_buffer.size();
                 d < e;
                 d += m_capacity_per_channel)
            {
                result.insert(result.end(), d, d + frames_to_consume);
            }
        }

        m_read_index.store(write_index, std::memory_order_release);

        return multichannel_buffer_t{m_num_channels, std::move(result)};
    }

private:
    static auto write_available(
            std::size_t const write_index,
            std::size_t const read_index,
            std::size_t const max_size) -> std::size_t
    {
        std::size_t const avail = read_index - write_index - 1;
        return write_index >= read_index ? avail + max_size : avail;
    }

    using write_stream_view = range::table_view<
            float,
            std::dynamic_extent,
            std::dynamic_extent,
            range::dynamic_stride,
            1>;

    std::size_t const m_num_channels;
    std::size_t const m_capacity_per_channel;
    alignas(thread::cache_line_size) std::atomic_size_t m_write_index{};
    alignas(thread::cache_line_size) std::atomic_size_t m_read_index{};
    alignas(thread::cache_line_size) std::vector<float> m_buffer;
    write_stream_view const m_buffer_multi_channel_view{
            m_buffer.data(),
            m_num_channels,
            m_capacity_per_channel,
            static_cast<range::table_view<float>::difference_type>(
                    m_capacity_per_channel),
            1};
};

} // namespace piejam::audio::engine
