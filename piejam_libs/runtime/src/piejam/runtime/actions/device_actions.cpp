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

#include <piejam/runtime/actions/device_actions.h>

#include <piejam/runtime/audio_state.h>

namespace piejam::runtime::actions
{

auto
refresh_devices::operator()(audio_state const& st) const -> audio_state
{
    return st;
}

auto
update_devices::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    new_st.pcm_devices = pcm_devices;
    new_st.input = input;
    new_st.output = output;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;
    new_st.mixer_state.inputs = piejam::audio::mixer::input_channels{
            new_st.input.hw_params->num_channels};

    return new_st;
}

auto
initiate_device_selection::operator()(audio_state const& st) const
        -> audio_state
{
    return st;
}

auto
select_device::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    (input ? new_st.input : new_st.output) = device;
    new_st.samplerate = samplerate;
    new_st.period_size = period_size;

    if (input)
    {
        new_st.mixer_state.inputs = piejam::audio::mixer::input_channels{
                new_st.input.hw_params->num_channels};
    }

    return new_st;
}

auto
select_samplerate::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const srs = samplerates_from_state(new_st);
    assert(index < srs.size());
    new_st.samplerate = srs[index];

    return new_st;
}

auto
select_period_size::operator()(audio_state const& st) const -> audio_state
{
    auto new_st = st;

    auto const pszs = period_sizes_from_state(new_st);
    assert(index < pszs.size());
    new_st.period_size = pszs[index];

    return new_st;
}

} // namespace piejam::runtime::actions
