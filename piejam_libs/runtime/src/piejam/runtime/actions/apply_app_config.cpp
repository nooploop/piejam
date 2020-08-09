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

#include <piejam/runtime/actions/apply_app_config.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

static void
update_channel(std::size_t& cur_ch, std::size_t const num_chs)
{
    if (cur_ch >= num_chs)
        cur_ch = algorithm::npos;
}

auto
apply_app_config::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto apply_bus_configs =
            [](auto& channels, auto const& configs, std::size_t num_ch) {
                channels.clear();
                for (auto const& bus_conf : configs)
                {
                    auto& bus = channels.emplace_back();
                    bus.name = bus_conf.name;
                    bus.type = bus_conf.bus_type;
                    bus.device_channels = bus_conf.channels;
                    update_channel(bus.device_channels.left, num_ch);
                    update_channel(bus.device_channels.right, num_ch);
                }
            };

    apply_bus_configs(
            new_st.mixer_state.inputs,
            conf.input_bus_config,
            new_st.input.hw_params->num_channels);

    apply_bus_configs(
            new_st.mixer_state.outputs,
            conf.output_bus_config,
            new_st.output.hw_params->num_channels);

    for (auto& bus : new_st.mixer_state.outputs)
        bus.type = audio::bus_type::stereo;

    return new_st;
}

} // namespace piejam::runtime::actions
