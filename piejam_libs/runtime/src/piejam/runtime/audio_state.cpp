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

#include <piejam/runtime/audio_state.h>

#include <algorithm>

namespace piejam::runtime
{

template <class Vector>
static auto
set_intersection(Vector const& in, Vector const& out)
{
    Vector result;
    std::ranges::set_intersection(in, out, std::back_inserter(result));
    return result;
}

auto
samplerates(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::samplerates_t
{
    return set_intersection(
            input_hw_params->samplerates,
            output_hw_params->samplerates);
}

auto
samplerates_from_state(audio_state const& state) -> audio::samplerates_t
{
    return samplerates(state.input.hw_params, state.output.hw_params);
}

auto
period_sizes(
        container::box<audio::pcm_hw_params> input_hw_params,
        container::box<audio::pcm_hw_params> output_hw_params)
        -> audio::period_sizes_t
{
    return set_intersection(
            input_hw_params->period_sizes,
            output_hw_params->period_sizes);
}

auto
period_sizes_from_state(audio_state const& state) -> audio::period_sizes_t
{
    return period_sizes(state.input.hw_params, state.output.hw_params);
}

static void
remove_fx_module(audio_state& st, fx::module_id id)
{
    if (fx::module const* const fx_mod = std::as_const(st.fx_modules)[id])
    {
        for (auto&& [key, fx_param] : fx_mod->parameters)
            st.float_params.remove(fx_param.id);

        st.fx_modules.remove(id);
    }
}

template <audio::bus_direction D>
void
add_mixer_bus(
        audio_state& st,
        std::string name,
        audio::bus_type type,
        channel_index_pair chs)
{
    auto volume_id = st.float_params.add(
            parameter::float_{.default_value = 1.f, .min = 0.f, .max = 8.f});
    auto pan_balance_id = st.float_params.add(
            parameter::float_{.default_value = 0.f, .min = -1.f, .max = 1.f});
    auto mute_id = st.bool_params.add(parameter::bool_{.default_value = false});
    auto level_id = st.levels.add(parameter::stereo_level{});

    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    bus_ids.emplace_back(st.mixer_state.buses.add(mixer::bus{
            .name = std::move(name),
            .volume = volume_id,
            .pan_balance = pan_balance_id,
            .mute = mute_id,
            .level = level_id,
            .type = type,
            .device_channels = std::move(chs)}));
    mixer::bus_ids<D>(st.mixer_state) = bus_ids;
}

template void add_mixer_bus<audio::bus_direction::input>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair);
template void add_mixer_bus<audio::bus_direction::output>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair);

template <audio::bus_direction D>
void
remove_mixer_bus(audio_state& st, std::size_t index)
{
    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    BOOST_ASSERT(index < bus_ids.size());
    mixer::bus_id const bus_id = bus_ids[index];

    if constexpr (D == audio::bus_direction::input)
    {
        if (st.mixer_state.input_solo_id == bus_id)
            st.mixer_state.input_solo_id = mixer::bus_id{};
    }

    mixer::bus const* const bus = std::as_const(st.mixer_state).buses[bus_id];
    BOOST_ASSERT(bus);
    st.float_params.remove(bus->volume);
    st.float_params.remove(bus->pan_balance);
    st.bool_params.remove(bus->mute);
    st.levels.remove(bus->level);

    for (auto&& fx_mod_id : bus->fx_chain)
        remove_fx_module(st, fx_mod_id);

    st.mixer_state.buses.remove(bus_id);
    bus_ids.erase(bus_ids.begin() + index);

    mixer::bus_ids<D>(st.mixer_state) = bus_ids;
}

template void
remove_mixer_bus<audio::bus_direction::input>(audio_state&, std::size_t);
template void
remove_mixer_bus<audio::bus_direction::output>(audio_state&, std::size_t);

template <audio::bus_direction D>
void
clear_mixer_buses(audio_state& st)
{
    std::size_t num_buses = mixer::bus_ids<D>(st.mixer_state)->size();
    while (num_buses--)
        remove_mixer_bus<D>(st, num_buses);
}

template void clear_mixer_buses<audio::bus_direction::input>(audio_state&);
template void clear_mixer_buses<audio::bus_direction::output>(audio_state&);

} // namespace piejam::runtime
