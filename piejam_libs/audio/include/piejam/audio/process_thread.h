// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/meta/type_traits.h>
#include <piejam/thread/affinity.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/priority.h>

#include <spdlog/spdlog.h>

#include <atomic>
#include <cassert>
#include <cstring>
#include <thread>
#include <type_traits>

namespace piejam::audio
{

class process_thread
{

public:
    process_thread() noexcept(meta::is_nothrow_default_constructible_v<
                              std::atomic_bool,
                              std::thread>) = default;

    ~process_thread() { stop(); }

    bool is_running() const noexcept
    {
        return m_running.load(std::memory_order_relaxed);
    }

    template <class Process>
    void start(thread::configuration const& conf, Process&& process)
    {
        assert(!m_running);

        m_running = true;
        m_thread = std::thread(
                [this,
                 conf,
                 fprocess = std::forward<Process>(process)]() mutable {
                    if (conf.affinity)
                        this_thread::set_affinity(*conf.affinity);
                    if (conf.priority)
                        this_thread::set_priority(*conf.priority);

                    try
                    {
                        while (m_running.load(std::memory_order_relaxed))
                        {
                            fprocess();
                        }
                    }
                    catch (std::exception const& err)
                    {
                        spdlog::error(err.what());
                    }
                });
    }

    void stop()
    {
        m_running = false;
        if (m_thread.joinable())
            m_thread.join();
    }

private:
    std::atomic_bool m_running{};
    std::thread m_thread;
};

} // namespace piejam::audio
