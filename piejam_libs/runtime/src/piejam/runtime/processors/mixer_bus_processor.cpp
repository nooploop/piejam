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

#include <piejam/runtime/processors/mixer_bus_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/lockstep_events.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/audio/pan.h>
#include <piejam/audio/types.h>

#include <boost/assert.hpp>

#include <array>

namespace piejam::runtime::processors
{

namespace
{

template <audio::bus_type C>
class mixer_bus_processor final : public audio::engine::named_processor
{
public:
    mixer_bus_processor(std::string_view const& name)
        : audio::engine::named_processor(name)
    {
    }

    auto type_name() const -> std::string_view override { return "mixer_bus"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{
                audio::engine::event_port{
                        std::in_place_type<float>,
                        C == audio::bus_type::mono ? "pan" : "balance"},
                audio::engine::event_port{std::in_place_type<float>, "volume"}};
        return s_ports;
    }

    auto event_outputs() const -> event_ports override
    {
        static std::array s_ports{
                audio::engine::event_port{std::in_place_type<float>, "gain L"},
                audio::engine::event_port{std::in_place_type<float>, "gain R"}};
        return s_ports;
    }

    static constexpr auto pan_balance_factors(float const pan_balance)
            -> audio::stereo_gain
    {
        return C == audio::bus_type::mono
                       ? audio::sinusoidal_constant_power_pan(pan_balance)
                       : audio::stereo_balance(pan_balance);
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        audio::engine::event_buffer<float> const& ev_buf_pan =
                ctx.event_inputs.get<float>(0);

        audio::engine::event_buffer<float> const& ev_buf_vol =
                ctx.event_inputs.get<float>(1);

        if (ev_buf_pan.empty() && ev_buf_vol.empty())
            return;

        audio::engine::event_buffer<float>& ev_buf_left =
                ctx.event_outputs.get<float>(0);
        audio::engine::event_buffer<float>& ev_buf_right =
                ctx.event_outputs.get<float>(1);

        std::tie(m_last_pan_balance, m_last_volume) =
                audio::engine::lockstep_events(
                        [&ev_buf_left, &ev_buf_right](
                                std::size_t const offset,
                                float const pan,
                                float const vol) {
                            auto const pbf = pan_balance_factors(pan);
                            ev_buf_left.insert(offset, pbf.left * vol);
                            ev_buf_right.insert(offset, pbf.right * vol);
                        },
                        std::tuple(m_last_pan_balance, m_last_volume),
                        ev_buf_pan,
                        ev_buf_vol);
    }

private:
    float m_last_pan_balance{0.f};
    float m_last_volume{1.f};
};

} // namespace

auto
make_mono_mixer_bus_processor(std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<mixer_bus_processor<audio::bus_type::mono>>(name);
}

auto
make_stereo_mixer_bus_processor(std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<mixer_bus_processor<audio::bus_type::stereo>>(name);
}

} // namespace piejam::runtime::processors
