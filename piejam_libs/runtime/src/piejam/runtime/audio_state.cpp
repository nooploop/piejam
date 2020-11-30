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

#include <piejam/indexed_access.h>
#include <piejam/runtime/fx/gain.h>
#include <piejam/runtime/fx/parameter.h>

#include <boost/range/algorithm_ext/erase.hpp>

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

static auto
make_fx_gain(audio_state& st)
{
    return st.fx_modules.add(fx::make_gain_module(st.float_params));
}

void
add_fx_module(audio_state& st, mixer::bus_id bus_id, fx::type fx_type)
{
    mixer::bus& bus = st.mixer_state.buses[bus_id];

    auto fx_chain = *bus.fx_chain;
    switch (fx_type)
    {
        case fx::type::gain:
            fx_chain.emplace_back(make_fx_gain(st));
            break;
    }
    bus.fx_chain = std::move(fx_chain);
}

void
remove_fx_module(audio_state& st, fx::module_id id)
{
    if (fx::module const* const fx_mod = std::as_const(st.fx_modules)[id])
    {
        for (auto&& [bus_id, bus] : st.mixer_state.buses)
        {
            if (std::ranges::find(*bus.fx_chain, id) != bus.fx_chain->end())
            {
                auto fx_chain = *bus.fx_chain;
                boost::remove_erase(fx_chain, id);
                bus.fx_chain = std::move(fx_chain);
            }
        }

        for (auto&& [key, fx_param] : *fx_mod->parameters)
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
        channel_index_pair const& chs)
{
    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    bus_ids.emplace_back(st.mixer_state.buses.add(mixer::bus{
            .name = std::move(name),
            .volume = st.float_params.add(parameter::float_{
                    .default_value = 1.f,
                    .min = 0.f,
                    .max = 4.f}),
            .pan_balance = st.float_params.add(parameter::float_{
                    .default_value = 0.f,
                    .min = -1.f,
                    .max = 1.f}),
            .mute = st.bool_params.add(
                    parameter::bool_{.default_value = false}),
            .level = st.levels.add(parameter::stereo_level{}),
            .type = type,
            .device_channels = chs,
            .fx_chain = {}}));
    mixer::bus_ids<D>(st.mixer_state) = bus_ids;
}

template void add_mixer_bus<audio::bus_direction::input>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&);
template void add_mixer_bus<audio::bus_direction::output>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&);

void
remove_mixer_bus(audio_state& st, mixer::bus_id const bus_id)
{
    if (st.mixer_state.input_solo_id == bus_id)
        st.mixer_state.input_solo_id = mixer::bus_id{};

    mixer::bus const* const bus = std::as_const(st.mixer_state).buses[bus_id];
    BOOST_ASSERT(bus);
    st.float_params.remove(bus->volume);
    st.float_params.remove(bus->pan_balance);
    st.bool_params.remove(bus->mute);
    st.levels.remove(bus->level);

    auto const fx_chain = *bus->fx_chain;
    for (auto&& fx_mod_id : fx_chain)
        remove_fx_module(st, fx_mod_id);

    if (st.fx_chain_bus == bus_id)
        st.fx_chain_bus = {};

    if (std::ranges::find(*st.mixer_state.inputs, bus_id) !=
        st.mixer_state.inputs->end())
    {
        auto bus_ids = *st.mixer_state.inputs;
        st.mixer_state.inputs = std::move(boost::remove_erase(bus_ids, bus_id));
    }
    else
    {
        auto bus_ids = *st.mixer_state.outputs;
        st.mixer_state.outputs =
                std::move(boost::remove_erase(bus_ids, bus_id));
    }

    st.mixer_state.buses.remove(bus_id);
}

template <audio::bus_direction D>
void
clear_mixer_buses(audio_state& st)
{
    auto const bus_ids = *mixer::bus_ids<D>(st.mixer_state);
    for (auto const bus_id : bus_ids)
        remove_mixer_bus(st, bus_id);
}

template void clear_mixer_buses<audio::bus_direction::input>(audio_state&);
template void clear_mixer_buses<audio::bus_direction::output>(audio_state&);

} // namespace piejam::runtime
