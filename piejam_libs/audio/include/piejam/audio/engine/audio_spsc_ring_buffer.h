// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/cache_line_size.h>

#include <boost/assert.hpp>

#include <atomic>
#include <concepts>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

class audio_spsc_ring_buffer
{
public:
    audio_spsc_ring_buffer(std::size_t const size)
        : m_capacity(size + 1)
        , m_buffer(m_capacity)

    {
    }

    auto write(std::span<float const> const& data) -> std::size_t
    {
        BOOST_ASSERT(!data.empty());

        std::size_t const write_index =
                m_write_index.load(std::memory_order_relaxed);
        std::size_t const read_index =
                m_read_index.load(std::memory_order_acquire);

        std::size_t const available =
                write_available(write_index, read_index, m_capacity);

        if (available == 0)
            return 0;

        std::size_t const write_size = std::min(data.size(), available);
        BOOST_ASSERT(write_size > 0);

        std::size_t new_write_index = write_index + write_size;

        auto data_end = std::next(data.begin(), write_size);
        auto write_out = std::next(m_buffer.begin(), write_index);
        if (new_write_index > m_capacity)
        {
            auto const data_middle =
                    std::next(data.begin(), m_capacity - write_index);

            std::copy(data.begin(), data_middle, std::move(write_out));
            std::copy(data_middle, std::move(data_end), m_buffer.begin());

            new_write_index -= m_capacity;
        }
        else
        {
            std::copy(data.begin(), std::move(data_end), std::move(write_out));

            if (new_write_index == m_capacity)
                new_write_index = 0;
        }

        m_write_index.store(new_write_index, std::memory_order_release);
        return write_size;
    }

    template <std::invocable<std::span<float const> const&> F>
    auto consume(F&& f)
    {
        std::size_t const write_index =
                m_write_index.load(std::memory_order_acquire);
        std::size_t const read_index =
                m_read_index.load(std::memory_order_relaxed);

        if (write_index == read_index)
            return;

        auto read_begin = std::next(m_buffer.begin(), read_index);
        auto read_end = std::next(m_buffer.begin(), write_index);
        if (write_index < read_index)
        {
            f(std::span(std::move(read_begin), m_buffer.end()));
            f(std::span(m_buffer.begin(), std::move(read_end)));
        }
        else
        {
            f(std::span(std::move(read_begin), std::move(read_end)));
        }

        m_read_index.store(write_index, std::memory_order_release);
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

    std::size_t const m_capacity;
    alignas(thread::cache_line_size) std::atomic_size_t m_write_index{};
    alignas(thread::cache_line_size) std::atomic_size_t m_read_index{};
    alignas(thread::cache_line_size) std::vector<float> m_buffer;
};

} // namespace piejam::audio::engine
