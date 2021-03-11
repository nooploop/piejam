// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/input_processor.h>
#include <piejam/audio/engine/output_processor.h>
#include <piejam/range/fwd.h>

#include <atomic>
#include <future>
#include <memory>

namespace piejam::audio::engine
{

class process
{
public:
    process(std::size_t num_device_input_channels,
            std::size_t num_device_output_channels);
    ~process();

    auto input(std::size_t const index) noexcept -> processor&
    {
        return m_input_procs[index];
    }

    auto output(std::size_t const index) noexcept -> processor&
    {
        return m_output_procs[index];
    }

    [[nodiscard]] bool swap_executor(std::unique_ptr<dag_executor>);

    void operator()(
            range::table_view<float const> const& in_audio,
            range::table_view<float> const& out_audio) noexcept;

private:
    std::vector<input_processor> m_input_procs;
    std::vector<output_processor> m_output_procs;

    std::unique_ptr<dag_executor> m_executor;

    std::atomic<dag_executor*> m_next_executor{};
    std::promise<std::unique_ptr<dag_executor>> m_prev_executor{};
};

} // namespace piejam::audio::engine
