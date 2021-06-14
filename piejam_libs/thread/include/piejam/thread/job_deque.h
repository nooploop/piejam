// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/cache_line_size.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <type_traits>

namespace piejam::thread
{

// Work-stealing queue from
// "Thread Scheduling for Multiprogrammed Multiprocessors"
// by Nimar S. Arora, Robert D. Blumofe, C. Greg Plaxton

template <class Job, std::size_t Capacity>
class job_deque
{
    static_assert(std::has_single_bit(Capacity)); // is power of two?

    using bottom_index = std::uint32_t;

    static_assert(std::atomic<bottom_index>::is_always_lock_free);

    struct top_index
    {
        std::uint16_t index{};
        std::uint16_t tag{};
    };

    static_assert(sizeof(top_index) == sizeof(bottom_index));
    static_assert(std::atomic<top_index>::is_always_lock_free);

public:
    constexpr auto capacity() const noexcept -> std::size_t { return Capacity; }

    template <std::input_iterator<> Iterator>
    void initialize(Iterator&& first, Iterator&& last)
    {
        m_top.store({}, std::memory_order_relaxed);
        auto it = std::copy(
                std::forward<Iterator>(first),
                std::forward<Iterator>(last),
                m_jobs.begin());
        m_bottom.store(
                static_cast<bottom_index>(std::distance(m_jobs.begin(), it)),
                std::memory_order_release);
        BOOST_ASSERT(it <= m_jobs.end());
    }

    template <std::ranges::input_range<> Range>
    void initialize(Range&& rng)
    {
        initialize(
                std::ranges::begin(std::forward<Range>(rng)),
                std::ranges::end(std::forward<Range>(rng)));
    }

    void push(Job* const job) noexcept
    {
        bottom_index const bottom = m_bottom.load(std::memory_order_acquire);
        BOOST_ASSERT(bottom < Capacity);
        m_jobs[bottom] = job;
        m_bottom.store(bottom + 1, std::memory_order_release);
    }

    auto pop() noexcept -> Job*
    {
        bottom_index const bottom = m_bottom.load(std::memory_order_acquire);
        if (bottom == 0)
            return nullptr;

        bottom_index const newBottom = bottom - 1;

        m_bottom.store(newBottom);

        Job* const job = m_jobs[newBottom];

        top_index top = m_top.load();
        if (newBottom > top.index)
            return job;

        top_index newTop;
        newTop.index = 0;
        newTop.tag = top.tag + 1;

        m_bottom.store(0);

        if (newBottom == top.index)
        {
            if (m_top.compare_exchange_strong(top, newTop))
                return job;
        }

        m_top.store(newTop, std::memory_order_release);
        return nullptr;
    }

    auto steal() noexcept -> Job*
    {
        top_index top = m_top.load(std::memory_order_acquire);

        if (m_bottom.load(std::memory_order_acquire) <= top.index)
            return nullptr;

        Job* const job = m_jobs[top.index];

        top_index newTop;
        newTop.index = top.index + 1;
        newTop.tag = top.tag + 1;

        if (m_top.compare_exchange_strong(top, newTop))
            return job;

        return nullptr;
    }

private:
    alignas(cache_line_size) std::atomic<bottom_index> m_bottom{};
    alignas(cache_line_size) std::atomic<top_index> m_top{};
    alignas(cache_line_size) std::array<Job*, Capacity> m_jobs{};
};

} // namespace piejam::thread
