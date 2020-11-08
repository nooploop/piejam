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

#include <piejam/runtime/audio_components/mixer_bus_processor.h>

#include <piejam/audio/engine/event_input_buffers.h>
#include <piejam/audio/engine/event_output_buffers.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/lockstep_events.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/process_context.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/npos.h>

#include <boost/assert.hpp>

#include <array>

namespace piejam::runtime::audio_components
{

namespace
{

class mute_solo_processor final : public audio::engine::named_processor
{
public:
    mute_solo_processor(
            std::size_t const solo_index,
            std::string_view const& name)
        : audio::engine::named_processor(name)
        , m_solo_index(solo_index)
    {
    }

    auto type_name() const -> std::string_view override { return "mute_solo"; }

    auto num_inputs() const -> std::size_t override { return 0; }
    auto num_outputs() const -> std::size_t override { return 0; }

    auto event_inputs() const -> event_ports override
    {
        static std::array s_ports{
                audio::engine::event_port(std::in_place_type<bool>, "mute"),
                audio::engine::event_port(
                        std::in_place_type<std::size_t>,
                        "solo_index")};
        return s_ports;
    }

    auto event_outputs() const -> event_ports override
    {
        static std::array s_ports{audio::engine::event_port{
                std::in_place_type<float>,
                "mute_amp"}};
        return s_ports;
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        audio::engine::event_buffer<bool> const& ev_buf_mute =
                ctx.event_inputs.get<bool>(0);

        audio::engine::event_buffer<std::size_t> const& ev_buf_solo =
                ctx.event_inputs.get<std::size_t>(1);

        if (ev_buf_mute.empty() && ev_buf_solo.empty())
            return;

        audio::engine::event_buffer<float>& ev_buf_mute_amp =
                ctx.event_outputs.get<float>(0);

        std::tie(m_last_mute, m_last_solo_index) =
                audio::engine::lockstep_events(
                        [this, &ev_buf_mute_amp](
                                std::size_t const offset,
                                bool const mute,
                                std::size_t const solo_index) {
                            if (solo_index == npos)
                            {
                                ev_buf_mute_amp.insert(offset, !mute);
                            }
                            else
                            {
                                ev_buf_mute_amp.insert(
                                        offset,
                                        solo_index == m_solo_index);
                            }
                        },
                        std::tuple(m_last_mute, m_last_solo_index),
                        ev_buf_mute,
                        ev_buf_solo);
    }

private:
    std::size_t const m_solo_index;

    bool m_last_mute{};
    std::size_t m_last_solo_index{npos};
};

} // namespace

auto
make_mute_solo_processor(
        std::size_t const solo_index,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::processor>
{
    return std::make_unique<mute_solo_processor>(solo_index, name);
}

} // namespace piejam::runtime::audio_components
