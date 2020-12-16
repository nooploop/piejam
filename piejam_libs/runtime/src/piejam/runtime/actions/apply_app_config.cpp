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
        cur_ch = npos;
}

template <audio::bus_direction D>
static auto
apply_bus_configs(
        state& st,
        std::vector<persistence::bus_config> const& configs,
        std::size_t const num_ch)
{
    clear_mixer_buses<D>(st);
    for (auto const& bus_conf : configs)
    {
        auto chs = bus_conf.channels;
        update_channel(chs.left, num_ch);
        update_channel(chs.right, num_ch);

        auto const type = D == audio::bus_direction::output
                                  ? audio::bus_type::stereo
                                  : bus_conf.bus_type;

        add_mixer_bus<D>(st, bus_conf.name, type, chs);
    }
}

auto
apply_app_config::reduce(state const& st) const -> state
{
    auto new_st = st;

    apply_bus_configs<audio::bus_direction::input>(
            new_st,
            conf.input_bus_config,
            new_st.input.hw_params->num_channels);

    apply_bus_configs<audio::bus_direction::output>(
            new_st,
            conf.output_bus_config,
            new_st.output.hw_params->num_channels);

    return new_st;
}

} // namespace piejam::runtime::actions
