// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/period_size.h>

#include <mipp.h>

#include <array>
#include <functional>
#include <span>
#include <vector>

namespace piejam::audio::engine
{

class processor;
class thread_context;

class processor_job final
{
public:
    using output_buffer_t = std::array<float, max_period_size.value()>;

    processor_job(processor& proc);

    auto result_ref(std::size_t index) const -> slice<float> const&;
    void connect_result(std::size_t index, slice<float> const& res);

    auto event_result_ref(std::size_t index) -> abstract_event_buffer const&;
    void connect_event_result(
            std::size_t index,
            abstract_event_buffer const& ev_buf);

    void clear_event_output_buffers();

    void operator()(thread_context const&);

private:
    processor& m_proc;
    mipp::vector<output_buffer_t> m_output_buffers;

    std::vector<std::reference_wrapper<slice<float> const>> m_inputs;
    std::vector<std::span<float>> m_outputs;
    std::vector<slice<float>> m_results;

    event_input_buffers m_event_inputs;
    event_output_buffers m_event_outputs;

    process_context m_process_context;
};

} // namespace piejam::audio::engine
