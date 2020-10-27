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

#include <piejam/audio/engine/amplify_processor.h>

#include <piejam/audio/engine/event_buffer_memory.h>
#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/processor.h>

#include <benchmark/benchmark.h>

namespace piejam::audio::engine
{

static void
BM_amplify_processor_process(benchmark::State& state)
{
    event_buffer_memory ev_buf_mem{1024};
    std::pmr::memory_resource* ev_mem{&ev_buf_mem.memory_resource()};
    event_buffer<float> ev_buf(ev_mem);
    event_input_buffers ev_bufs(1);
    ev_bufs.set(0, ev_buf);
    event_output_buffers ev_out_bufs;
    std::array<float, 1024> audio_in_mem;
    audio_in_mem.fill(.58f);
    std::vector<std::span<float const>> in_buf_spans{audio_in_mem};
    std::vector<std::reference_wrapper<std::span<float const> const>> in_bufs(
            in_buf_spans.begin(),
            in_buf_spans.end());
    std::array<float, 1024> audio_out_mem;
    std::vector<std::span<float>> out_buf_spans{audio_out_mem};
    std::vector<std::span<float const>> res_spans{1};

    process_context
            ctx{in_bufs, out_buf_spans, res_spans, ev_bufs, ev_out_bufs, 1};

    // set amplify factor
    in_buf_spans[0] = {}; // silence the input buffer
    ev_buf.insert(0, .23f);
    std::unique_ptr<processor> sut = make_amplify_processor();
    sut->process(ctx);
    ev_buf.clear();

    std::size_t const buffer_size = state.range(0);
    ctx.buffer_size = buffer_size;
    in_buf_spans[0] = {audio_in_mem.data(), buffer_size};
    out_buf_spans[0] = {audio_out_mem.data(), buffer_size};

    for (auto _ : state)
        sut->process(ctx);
}

BENCHMARK(BM_amplify_processor_process)->RangeMultiplier(2)->Range(1, 1024);

} // namespace piejam::audio::engine
