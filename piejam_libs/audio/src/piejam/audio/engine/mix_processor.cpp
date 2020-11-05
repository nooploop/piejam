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
#include <piejam/npos.h>

#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

namespace
{

template <std::size_t N>
auto
mix(process_context const& ctx)
{
    return add(ctx.inputs[N - 1].get(), mix<N - 1>(ctx), ctx.outputs[0]);
}

template <>
auto
mix<2>(process_context const& ctx)
{
    return add(ctx.inputs[1].get(), ctx.inputs[0].get(), ctx.outputs[0]);
}

template <>
auto
mix<npos>(process_context const& ctx)
{
    audio_slice res;

    auto const out = ctx.outputs[0];

    for (audio_slice const& in : ctx.inputs)
    {
        if (is_silence(in))
            continue;

        res = add(in, res, out);
    }

    return res;
}

template <std::size_t NumInputs>
class mix_processor final : public named_processor
{
public:
    mix_processor(std::string_view const& name) requires(NumInputs != npos)
        : named_processor(name)
        , m_num_inputs(NumInputs)
    {
    }

    mix_processor(
            std::size_t const num_inputs,
            std::string_view const& name) requires(NumInputs == npos)
        : named_processor(name)
        , m_num_inputs(num_inputs)
    {
    }

    auto type_name() const -> std::string_view override { return "mix"; }

    auto num_inputs() const -> std::size_t override
    {
        return NumInputs != npos ? NumInputs : m_num_inputs;
    }
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

        ctx.results[0] = mix<NumInputs>(ctx);
    }

private:
    std::size_t const m_num_inputs{};
};

} // namespace

auto
make_mix_processor(std::size_t const num_inputs, std::string_view const& name)
        -> std::unique_ptr<processor>
{
    switch (num_inputs)
    {
        case 2:
            return std::make_unique<mix_processor<2>>(name);
        case 3:
            return std::make_unique<mix_processor<3>>(name);
        case 4:
            return std::make_unique<mix_processor<4>>(name);
        case 5:
            return std::make_unique<mix_processor<5>>(name);
        case 6:
            return std::make_unique<mix_processor<6>>(name);
        case 7:
            return std::make_unique<mix_processor<7>>(name);
        case 8:
            return std::make_unique<mix_processor<8>>(name);
        default:
            BOOST_ASSERT(num_inputs > 1);
            return std::make_unique<mix_processor<npos>>(num_inputs, name);
    }
}

} // namespace piejam::audio::engine
