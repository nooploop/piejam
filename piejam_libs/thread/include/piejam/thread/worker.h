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

#include <piejam/thread/affinity.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/priority.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace piejam::thread
{

//! A thread which becomes a task assigned. The needs to be
//! woken up to execute the task. When the work on the task is done,
//! the thread yields the cpu and waits for the next wakeup call.
class worker
{
public:
    template <class F>
    worker(F&& f, thread::configuration conf = {})
        : m_thread([this,
                    ff = std::forward<F>(f),
                    conf = std::move(conf)]() mutable {
            if (conf.affinity)
                this_thread::set_affinity(*conf.affinity);
            if (conf.priority)
                this_thread::set_priority(*conf.priority);

            while (m_running.load(std::memory_order_consume))
            {
                {
                    std::unique_lock<std::mutex> lock(m_work_mutex);
                    m_work_cond_var.wait(lock, [this]() { return m_work; });
                    m_work = false;
                }

                if (!m_running.load(std::memory_order_consume))
                    break;

                ff();
            }
        })
    {
    }

    ~worker() { stop(); }

    void wakeup()
    {
        {
            std::lock_guard<std::mutex> lock(m_work_mutex);
            m_work = true;
        }

        m_work_cond_var.notify_one();
    }

    void stop()
    {
        m_running.store(false, std::memory_order_release);

        wakeup();

        m_thread.join();
    }

private:
    std::atomic_bool m_running{true};
    std::mutex m_work_mutex;
    std::condition_variable m_work_cond_var;
    bool m_work{false};
    std::thread m_thread;
};

} // namespace piejam::thread
