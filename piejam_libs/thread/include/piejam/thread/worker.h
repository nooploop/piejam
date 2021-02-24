// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/affinity.h>
#include <piejam/thread/configuration.h>
#include <piejam/thread/priority.h>

#include <atomic>
#include <concepts>
#include <condition_variable>
#include <functional>
#include <mutex>
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
                {
                    std::unique_lock<std::mutex> lock(m_work_mutex);
                    m_work_cond_var.wait(lock, [this]() { return m_work; });
                    m_work = false;
                }

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
        {
            std::lock_guard<std::mutex> lock(m_work_mutex);
            m_task = std::forward<F>(task);
            m_work = true;
        }

        m_work_cond_var.notify_one();
    }

    void stop()
    {
        m_running.store(false, std::memory_order_release);

        wakeup([]() {});

        m_thread.join();
    }

private:
    std::atomic_bool m_running{true};
    std::mutex m_work_mutex;
    std::condition_variable m_work_cond_var;
    bool m_work{false};
    task_t m_task{[]() {}};
    std::thread m_thread;
};

} // namespace piejam::thread
