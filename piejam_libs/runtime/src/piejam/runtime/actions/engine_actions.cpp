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

#include <piejam/runtime/actions/engine_actions.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

auto
set_input_channel_gain::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());
    auto& channel = new_st.mixer_state.inputs[index];
    channel.gain = gain;

    return new_st;
}

auto
set_input_channel_pan::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.inputs.size());
    auto& channel = new_st.mixer_state.inputs[index];
    channel.pan = pan;

    return new_st;
}

auto
set_output_channel_gain::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.outputs.size());
    new_st.mixer_state.outputs[index].gain = gain;

    return new_st;
}

auto
set_output_channel_balance::operator()(audio_state const& st) const
        -> audio_state
{
    auto new_st = st;

    assert(index < new_st.mixer_state.outputs.size());
    new_st.mixer_state.outputs[index].balance = balance;

    return new_st;
}

auto
request_levels_update::operator()(audio_state const& st) const -> audio_state
{
    return st;
}

auto
update_levels::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const update = [](auto& channels, auto const& levels) {
        std::size_t const num_levels = levels.size();
        assert(num_levels == channels.size());
        for (std::size_t index = 0; index < num_levels; ++index)
            channels[index].level = levels[index];
    };

    update(new_st.mixer_state.inputs, in_levels);
    update(new_st.mixer_state.outputs, out_levels);

    return new_st;
}

auto
request_info_update::operator()(audio_state const& st) const -> audio_state
{
    return st;
}

auto
update_info::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    new_st.xruns = xruns;
    new_st.cpu_load = cpu_load;

    return new_st;
}

} // namespace piejam::runtime::actions
