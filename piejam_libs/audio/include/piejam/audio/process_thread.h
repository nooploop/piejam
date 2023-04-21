// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/configuration.h>
#include <piejam/type_traits.h>

#include <boost/assert.hpp>

#include <atomic>
#include <thread>
#include <type_traits>

namespace piejam::audio
{

class process_thread
{

public:
    process_thread() noexcept(
            is_nothrow_default_constructible_v<std::atomic_bool, std::thread>) =
            default;

    ~process_thread()
    {
        stop();
    }

    [[nodiscard]] auto is_running() const noexcept -> bool
    {
        return m_running.load(std::memory_order_relaxed);
    }

    [[nodiscard]] auto error() const -> std::error_condition const&
    {
        BOOST_ASSERT(!m_running);
        return m_error;
    }

    template <class Process>
    void start(thread::configuration const& conf, Process&& process)
    {
        BOOST_ASSERT(!m_running);

        m_error = {};
        m_running = true;
        m_thread = std::thread(
                [this,
                 conf,
                 fprocess = std::forward<Process>(process)]() mutable {
                    conf.apply();

                    static_assert(!std::is_reference_v<decltype(fprocess)>);

                    while (m_running.load(std::memory_order_relaxed))
                    {
                        if (std::error_condition err = fprocess())
                        {
                            m_error = err;
                            m_running.store(false, std::memory_order_relaxed);
                        }
                    }
                });
    }

    void stop()
    {
        m_running = false;
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

private:
    std::error_condition m_error;
    std::atomic_bool m_running{};
    std::thread m_thread;
};

} // namespace piejam::audio
