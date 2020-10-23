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

#include <piejam/audio/engine/mix_processor.h>

#include <piejam/audio/engine/processor.h>

#include <algorithm>
#include <cassert>

namespace piejam::audio::engine
{

namespace
{

class mix_processor final : public processor
{
public:
    mix_processor(std::size_t num_inputs);

    auto num_inputs() const -> std::size_t override { return m_num_inputs; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto num_event_inputs() const -> std::size_t override { return 0; }
    auto num_event_outputs() const -> std::size_t override { return 0; }

    void create_event_output_buffers(
            event_output_buffer_factory const&) const override
    {
    }

    void
    process(input_buffers_t const&,
            output_buffers_t const&,
            result_buffers_t const&,
            event_input_buffers const&,
            event_output_buffers const&) override;

private:
    std::size_t const m_num_inputs{};
};

mix_processor::mix_processor(std::size_t num_inputs)
    : m_num_inputs(num_inputs)
{
    assert(m_num_inputs > 1);
}

void
mix_processor::process(
        input_buffers_t const& in_bufs,
        output_buffers_t const& out_bufs,
        result_buffers_t const& result_bufs,
        event_input_buffers const&,
        event_output_buffers const&)
{
    assert(in_bufs.size() == m_num_inputs);
    assert(out_bufs.size() == 1 && !out_bufs[0].empty());
    assert(result_bufs.size() == 1);

    std::size_t mixed = 0;
    for (std::span<float const> const& in : in_bufs)
    {
        if (in.empty())
            continue;

        assert(in.size() == out_bufs[0].size());

        if (mixed > 1)
        {
            // we already mixed atleast two channels,
            // so additionally mix this input.
            std::transform(
                    in.begin(),
                    in.end(),
                    out_bufs[0].begin(),
                    out_bufs[0].begin(),
                    std::plus<float>{});
        }
        else if (mixed == 1)
        {
            // we already mixed one channel, which is set as result.
            // we mix this input and current result into the output.
            std::transform(
                    in.begin(),
                    in.end(),
                    result_bufs[0].begin(),
                    out_bufs[0].begin(),
                    std::plus<float>{});
            result_bufs[0] = out_bufs[0];
            ++mixed;
        }
        else
        {
            // we didn't mixed anything yet, so we pass this input as result
            result_bufs[0] = in;
            ++mixed;
        }
    }

    if (mixed == 0)
        result_bufs[0] = {};
}

} // namespace

auto
make_mix_processor(std::size_t const num_inputs) -> std::unique_ptr<processor>
{
    return std::make_unique<mix_processor>(num_inputs);
}

} // namespace piejam::audio::engine
