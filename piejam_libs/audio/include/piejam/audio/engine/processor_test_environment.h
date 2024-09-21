// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_buffer.h>
#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>

#include <mipp.h>

#include <boost/polymorphic_cast.hpp>

#include <vector>

namespace piejam::audio::engine
{

struct processor_test_environment
{
    processor_test_environment(processor& proc, std::size_t buffer_size)
        : ctx({.event_inputs = event_inputs,
               .event_outputs = event_outputs,
               .buffer_size = buffer_size})
    {
        for (std::size_t n = 0, e = proc.num_inputs(); n < e; ++n)
        {
            audio_inputs.emplace_back();
            audio_in_slices.emplace_back(audio_inputs.back());
        }
        ctx.inputs = audio_in_slices;

        for (std::size_t n = 0, e = proc.num_outputs(); n < e; ++n)
        {
            audio_out_bufs.emplace_back(buffer_size, 0.f);
            audio_outputs.emplace_back(audio_out_bufs.back());
            audio_results.emplace_back();
        }
        ctx.outputs = audio_outputs;
        ctx.results = audio_results;

        {
            std::size_t buffer_index{};
            for (event_port const& p : proc.event_inputs())
            {
                event_inputs.add(p);
                ev_in_bufs.push_back(p.make_event_buffer(ev_mem_res));
                event_inputs.set(buffer_index++, *ev_in_bufs.back());
            }
        }

        event_outputs.set_event_memory(ev_mem_res);
        for (event_port const& p : proc.event_outputs())
        {
            event_outputs.add(p);
        }
    }

    template <class T>
    void
    insert_input_event(std::size_t buffer_index, std::size_t offset, T&& value)
    {
        BOOST_ASSERT(buffer_index < ev_in_bufs.size());
        boost::polymorphic_downcast<event_buffer<T>*>(
                ev_in_bufs[buffer_index].get())
                ->insert(offset, std::forward<T>(value));
    }

private:
    event_buffer_memory ev_mem{1 << 16};
    std::pmr::memory_resource* ev_mem_res{&ev_mem.memory_resource()};
    std::vector<std::reference_wrapper<audio_slice const>> audio_in_slices;
    std::vector<std::vector<float, mipp::allocator<float>>> audio_out_bufs;
    std::vector<std::unique_ptr<abstract_event_buffer>> ev_in_bufs;

public:
    std::vector<audio_slice> audio_inputs;
    std::vector<std::span<float>> audio_outputs;
    std::vector<audio_slice> audio_results;
    event_input_buffers event_inputs;
    event_output_buffers event_outputs;
    process_context ctx;
};

} // namespace piejam::audio::engine
