// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/processor_job.h>

#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/slice.h>
#include <piejam/audio/engine/thread_context.h>

#include <boost/align/is_aligned.hpp>
#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

static auto
empty_result_ref() -> std::reference_wrapper<audio_slice const>
{
    static audio_slice res;
    return std::cref(res);
}

processor_job::processor_job(processor& proc)
    : m_proc(proc)
    , m_output_buffers(m_proc.num_outputs(), output_buffer_t{})
    , m_inputs(m_proc.num_inputs(), empty_result_ref())
    , m_outputs(m_output_buffers.begin(), m_output_buffers.end())
    , m_results(m_proc.num_outputs())
    , m_process_context(
              {m_inputs, m_outputs, m_results, m_event_inputs, m_event_outputs})
{
    BOOST_ASSERT((std::ranges::all_of(
            m_output_buffers,
            [](auto p) {
                return boost::alignment::is_aligned(p, XSIMD_DEFAULT_ALIGNMENT);
            },
            [](auto const& b) { return b.data(); })));
    for (event_port const& port : m_proc.event_inputs())
        m_event_inputs.add(port);
    BOOST_ASSERT(m_proc.event_inputs().size() == m_event_inputs.size());
    for (event_port const& port : m_proc.event_outputs())
        m_event_outputs.add(port);
    BOOST_ASSERT(m_proc.event_outputs().size() == m_event_outputs.size());
}

auto
processor_job::result_ref(std::size_t const index) const -> audio_slice const&
{
    return m_results[index];
}

auto
processor_job::event_result_ref(std::size_t const index)
        -> abstract_event_buffer const&
{
    return m_event_outputs.get(index);
}

void
processor_job::connect_result(std::size_t const index, audio_slice const& res)
{
    BOOST_ASSERT(index < m_inputs.size());
    m_inputs[index] = std::ref(res);
}

void
processor_job::connect_event_result(
        std::size_t const index,
        abstract_event_buffer const& ev_buf)
{
    m_event_inputs.set(index, ev_buf);
}

void
processor_job::clear_event_output_buffers()
{
    m_event_outputs.clear_buffers();
}

void
processor_job::operator()(
        thread_context const& ctx,
        std::size_t const buffer_size)
{
    // set output buffer sizes
    for (std::span<float>& out : m_outputs)
        out = {out.data(), buffer_size};

    BOOST_ASSERT(ctx.event_memory);
    m_event_outputs.set_event_memory(ctx.event_memory);

    m_process_context.buffer_size = buffer_size;
    m_proc.process(m_process_context);
}

} // namespace piejam::audio::engine
