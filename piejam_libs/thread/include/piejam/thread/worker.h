// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/affinity.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/priority.h>
#include <piejam/thread/semaphore.h>

#include <atomic>
#include <concepts>
#include <functional>
#include <thread>

namespace piejam::thread
{

//! A thread which gets a task assigned. The worker needs to be
//! woken up to execute the task. When the work on the task is done,
//! the thread yields the cpu and waits for the next wakeup call.
class worker
{
public:
    using task_t = std::function<void()>;

    worker(thread::configuration conf = {})
        : m_thread([this, conf = std::move(conf)]() mutable {
            if (conf.affinity)
                this_thread::set_affinity(*conf.affinity);
            if (conf.priority)
                this_thread::set_priority(*conf.priority);

            while (m_running.load(std::memory_order_consume))
            {
                m_sem.acquire();

                if (!m_running.load(std::memory_order_consume))
                    break;

                m_task();
            }
        })
    {
    }

    ~worker() { stop(); }

    template <std::invocable<> F>
    void wakeup(F&& task)
    {
        m_task = std::forward<F>(task);
        m_sem.release();
    }

    void stop()
    {
        m_running.store(false, std::memory_order_release);

        wakeup([]() {});

        m_thread.join();
    }

private:
    std::atomic_bool m_running{true};
    semaphore m_sem;

    task_t m_task{[]() {}};
    std::thread m_thread;
};

} // namespace piejam::thread
