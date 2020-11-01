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

#include <piejam/audio/engine/audio_slice.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/functional/overload.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

namespace
{

class mix_processor final : public named_processor
{
public:
    mix_processor(std::size_t num_inputs, std::string_view const& name)
        : named_processor(name)
        , m_num_inputs(num_inputs)
    {
        BOOST_ASSERT(m_num_inputs > 1);
    }

    auto type_name() const -> std::string_view override { return "mix"; }

    auto num_inputs() const -> std::size_t override { return m_num_inputs; }
    auto num_outputs() const -> std::size_t override { return 1; }

    auto event_inputs() const -> event_ports override { return {}; }
    auto event_outputs() const -> event_ports override { return {}; }

    void create_event_output_buffers(
            event_output_buffer_factory const&) const override
    {
    }

    void process(process_context const& ctx) override
    {
        verify_process_context(*this, ctx);
        std::invoke(m_process_fn, this, ctx);
    }

private:
    void process_2_inputs(process_context const& ctx)
    {
        ctx.results[0] =
                add(ctx.inputs[0].get(), ctx.inputs[1].get(), ctx.outputs[0]);
    }

    void process_3_inputs(process_context const& ctx)
    {
        ctx.results[0] = add(
                ctx.inputs[2].get(),
                add(ctx.inputs[0].get(), ctx.inputs[1].get(), ctx.outputs[0]),
                ctx.outputs[0]);
    }

    void process_4_inputs(process_context const& ctx)
    {
        ctx.results[0] = add(
                add(ctx.inputs[0].get(), ctx.inputs[1].get(), ctx.outputs[0]),
                add(ctx.inputs[2].get(), ctx.inputs[3].get(), ctx.outputs[0]),
                ctx.outputs[0]);
    }

    void process_multiple_inputs(process_context const& ctx)
    {
        auto& res = ctx.results[0];
        res = {};

        auto const out = ctx.outputs[0];

        for (audio_slice const& in : ctx.inputs)
        {
            if (is_silence(in))
                continue;

            res = add(in, res, out);
        }
    }

    using process_fn_t = decltype(&mix_processor::process);
    constexpr auto get_process_fn(std::size_t const num_inputs) noexcept
            -> process_fn_t
    {
        switch (num_inputs)
        {
            case 2:
                return &mix_processor::process_2_inputs;
            case 3:
                return &mix_processor::process_3_inputs;
            case 4:
                return &mix_processor::process_4_inputs;
            default:
                return &mix_processor::process_multiple_inputs;
        }
    }

    std::size_t const m_num_inputs{};
    process_fn_t const m_process_fn{get_process_fn(m_num_inputs)};
};

} // namespace

auto
make_mix_processor(std::size_t const num_inputs, std::string_view const& name)
        -> std::unique_ptr<processor>
{
    return std::make_unique<mix_processor>(num_inputs, name);
}

} // namespace piejam::audio::engine
