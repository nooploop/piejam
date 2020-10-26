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

#include <piejam/audio/engine/processor_job.h>

#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/thread_context.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

static auto
empty_result_ref() -> std::reference_wrapper<std::span<float const> const>
{
    static std::span<float const> res;
    return std::cref(res);
}

processor_job::processor_job(
        processor& proc,
        std::size_t const& buffer_size_ref)
    : m_proc(proc)
    , m_buffer_size(buffer_size_ref)
    , m_output_buffers(m_proc.num_outputs(), output_buffer_t{})
    , m_inputs(m_proc.num_inputs(), empty_result_ref())
    , m_outputs(m_output_buffers.begin(), m_output_buffers.end())
    , m_results(m_proc.num_outputs())
    , m_event_inputs(m_proc.num_event_inputs())
    , m_process_context(
              {m_inputs, m_outputs, m_results, m_event_inputs, m_event_outputs})
{
    m_proc.create_event_output_buffers(m_event_outputs);
    BOOST_ASSERT(m_proc.num_event_outputs() == m_event_outputs.size());
}

auto
processor_job::result_ref(std::size_t const index) const
        -> std::span<float const> const&
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
processor_job::connect_result(
        std::size_t const index,
        std::span<float const> const& res)
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
processor_job::operator()(thread_context const& ctx)
{
    // set output buffer sizes
    for (std::span<float>& out : m_outputs)
        out = {out.data(), m_buffer_size};

    // initialize results with output buffers
    std::ranges::copy(m_outputs, m_results.begin());

    BOOST_ASSERT(ctx.event_memory);
    m_event_outputs.set_event_memory(ctx.event_memory);

    m_process_context.buffer_size = m_buffer_size;
    m_proc.process(m_process_context);
}

} // namespace piejam::audio::engine
