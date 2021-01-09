// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/math.h>

#include <array>
#include <atomic>
#include <limits>
#include <type_traits>

namespace piejam::thread
{

template <class Job, std::size_t Capacity>
class job_deque
{
    static_assert(math::is_power_of_two(Capacity));
    static constexpr std::size_t mask = Capacity - 1u;

    using signed_index_t = std::make_signed_t<std::size_t>;

public:
    constexpr auto capacity() const noexcept -> std::size_t { return Capacity; }

    //! Can only be called by owner.
    auto size() const noexcept -> std::size_t
    {
        return m_bottom.load(std::memory_order_relaxed) -
               m_top.load(std::memory_order_relaxed);
    }

    //! Can only be called by owner.
    void push(Job* job)
    {
        std::size_t const bottom = m_bottom.load(std::memory_order_acquire);
        m_jobs[bottom & mask] = job;
        m_bottom.store(bottom + 1, std::memory_order_release);
    }

    //! Can only be called by owner.
    auto pop() -> Job*
    {
        std::size_t const bottom = m_bottom.load(std::memory_order_acquire) - 1;
        m_bottom.store(bottom, std::memory_order_release);
        std::size_t top = m_top.load(std::memory_order_acquire);

        if (static_cast<signed_index_t>(top) <=
            static_cast<signed_index_t>(bottom))
        {
            Job* job = m_jobs[bottom & mask];
            if (top != bottom)
            {
                return job;
            }
            else
            {
                std::size_t expected_top = top;
                if (!m_top.compare_exchange_strong(
                            expected_top,
                            top + 1,
                            std::memory_order_acq_rel))
                {
                    job = nullptr;
                }

                m_bottom.store(top + 1, std::memory_order_release);
                return job;
            }
        }
        else
        {
            m_bottom.store(top, std::memory_order_release);
            return nullptr;
        }
    }

    //! Can only be called by thieves.
    auto steal() -> Job*
    {
        std::size_t top = m_top.load(std::memory_order_acquire);
        std::size_t const bottom = m_bottom.load(std::memory_order_consume);

        if (static_cast<signed_index_t>(top) <
            static_cast<signed_index_t>(bottom))
        {
            Job* const job = m_jobs[top & mask];
            if (m_top.compare_exchange_strong(
                        top,
                        top + 1,
                        std::memory_order_release))
            {
                return job;
            }
        }

        return nullptr;
    }

    //! Should only be called when not in use.
    void reset()
    {
        m_bottom.store(0, std::memory_order_relaxed);
        m_top.store(0, std::memory_order_relaxed);
    }

private:
    std::atomic_size_t m_bottom{};
    std::array<Job*, Capacity> m_jobs{};
    std::atomic_size_t m_top{};
};

} // namespace piejam::thread
