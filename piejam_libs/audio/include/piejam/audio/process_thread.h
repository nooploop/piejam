// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/alloc_debug.h>

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

    [[nodiscard]]
    auto is_running() const noexcept -> bool
    {
        return m_thread.joinable();
    }

    [[nodiscard]]
    auto error() const -> std::error_condition const&
    {
        BOOST_ASSERT(!is_running());
        return m_error;
    }

    template <class Process>
    void start(thread::configuration const& conf, Process&& process)
    {
        BOOST_ASSERT(!is_running());

        m_error = {};
        m_thread = std::jthread(
                [this, conf, fprocess = std::forward<Process>(process)](
                        std::stop_token stop_token) mutable {
                    conf.apply();
                    prohibit_dynamic_memory_allocation();

                    static_assert(!std::is_reference_v<decltype(fprocess)>);

                    while (!stop_token.stop_requested())
                    {
                        if (std::error_condition err = fprocess())
                        {
                            m_error = err;
                            break;
                        }
                    }
                });
    }

    void stop()
    {
        m_thread.request_stop();
    }

private:
    std::error_condition m_error;
    std::jthread m_thread;
};

} // namespace piejam::audio
