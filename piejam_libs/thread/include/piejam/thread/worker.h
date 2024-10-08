// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/thread/configuration.h>

#include <concepts>
#include <functional>
#include <semaphore>
#include <thread>

namespace piejam::thread
{

//! Single task worker thread.
class worker
{
public:
    using task_t = std::function<void()>;

    worker(thread::configuration conf = {})
        : m_thread([this, conf = std::move(conf)](std::stop_token stoken) {
            conf.apply();

            while (true)
            {
                m_sem_work.acquire();

                if (stoken.stop_requested())
                {
                    break;
                }

                m_task();

                m_sem_finished.release();
            }
        })
    {
    }

    worker(worker const&) = delete;
    worker(worker&&) = delete;

    ~worker()
    {
        m_sem_finished.acquire();
        m_thread.request_stop();
        m_sem_work.release();
    }

    auto operator=(worker const&) -> worker& = delete;
    auto operator=(worker&&) -> worker& = delete;

    template <std::invocable<> F>
    void wakeup(F&& task) noexcept
    {
        m_sem_finished.acquire();
        m_task = std::forward<F>(task);
        m_sem_work.release();
    }

private:
    std::binary_semaphore m_sem_work{0};
    std::binary_semaphore m_sem_finished{1};

    task_t m_task{[]() {}};
    std::jthread m_thread;
};

} // namespace piejam::thread
