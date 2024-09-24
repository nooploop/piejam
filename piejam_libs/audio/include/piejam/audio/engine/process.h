// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph.h>

#include <atomic>
#include <future>
#include <memory>

namespace piejam::audio::engine
{

class process
{
public:
    process();
    ~process();

    [[nodiscard]]
    bool swap_executor(std::unique_ptr<dag_executor>);

    void operator()(std::size_t buffer_size) noexcept;

private:
    std::unique_ptr<dag_executor> m_executor;

    std::atomic<dag_executor*> m_next_executor{};
    std::promise<std::unique_ptr<dag_executor>> m_prev_executor{};
};

} // namespace piejam::audio::engine
