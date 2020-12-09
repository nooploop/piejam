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

#include <piejam/algorithm/contains.h>
#include <piejam/audio/ladspa/port_descriptor.h>
#include <piejam/functional/overload.h>
#include <piejam/indexed_access.h>
#include <piejam/runtime/fx/gain.h>
#include <piejam/runtime/fx/ladspa.h>
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
make_fx_gain(
        fx::modules_t& fx_modules,
        fx::parameters_t& fx_params,
        parameter_maps& params)
{
    return fx_modules.add(
            fx::make_gain_module(fx_params, params.get_map<float_parameter>()));
}

void
add_internal_fx_module(
        audio_state& st,
        mixer::bus_id bus_id,
        fx::internal fx_type)
{
    std::tie(st.mixer_state.buses, st.fx_modules, st.fx_parameters) =
            [bus_id, fx_type](
                    mixer::buses_t buses,
                    fx::chain_t fx_chain,
                    fx::modules_t fx_modules,
                    fx::parameters_t fx_params,
                    parameter_maps& params) {
                mixer::bus& bus = buses[bus_id];

                switch (fx_type)
                {
                    case fx::internal::gain:
                        fx_chain.emplace_back(
                                make_fx_gain(fx_modules, fx_params, params));
                        break;
                }

                bus.fx_chain = std::move(fx_chain);

                return std::tuple(
                        std::move(buses),
                        std::move(fx_modules),
                        std::move(fx_params));
            }(st.mixer_state.buses,
              st.mixer_state.buses.get()[bus_id]->fx_chain,
              st.fx_modules,
              st.fx_parameters,
              st.params);
}

void
add_ladspa_fx_module(
        audio_state& st,
        mixer::bus_id bus_id,
        fx::ladspa_instance_id instance_id,
        std::string const& name,
        std::span<audio::ladspa::port_descriptor const> const& control_inputs)
{
    std::tie(st.mixer_state.buses, st.fx_modules, st.fx_parameters) =
            [bus_id, instance_id, &name, control_inputs](
                    mixer::buses_t buses,
                    fx::chain_t fx_chain,
                    fx::modules_t fx_modules,
                    fx::parameters_t fx_params,
                    parameter_maps& params) {
                mixer::bus& bus = buses[bus_id];

                fx_chain.emplace_back(fx_modules.add(fx::make_ladspa_module(
                        instance_id,
                        name,
                        control_inputs,
                        fx_params,
                        params.get_map<float_parameter>(),
                        params.get_map<int_parameter>(),
                        params.get_map<bool_parameter>())));

                bus.fx_chain = std::move(fx_chain);

                return std::tuple(
                        std::move(buses),
                        std::move(fx_modules),
                        std::move(fx_params));
            }(st.mixer_state.buses,
              st.mixer_state.buses.get()[bus_id]->fx_chain,
              st.fx_modules,
              st.fx_parameters,
              st.params);
}

void
remove_fx_module(audio_state& st, fx::module_id id)
{
    if (fx::module const* const fx_mod = st.fx_modules.get()[id])
    {
        st.mixer_state.buses = [id](mixer::buses_t buses) {
            for (auto&& [bus_id, bus] : buses)
            {
                if (algorithm::contains(*bus.fx_chain, id))
                {
                    auto fx_chain = *bus.fx_chain;
                    boost::remove_erase(fx_chain, id);
                    bus.fx_chain = std::move(fx_chain);
                }
            }

            return buses;
        }(st.mixer_state.buses);

        for (auto&& [key, fx_param_id] : *fx_mod->parameters)
        {
            std::visit([&st](auto&& id) { st.params.remove(id); }, fx_param_id);
            st.fx_parameters = [](fx::parameters_t fx_params,
                                  fx::parameter_id id) {
                fx_params.erase(id);
                return fx_params;
            }(st.fx_parameters, fx_param_id);
        }

        st.fx_modules = [id](fx::modules_t fx_modules) {
            fx_modules.remove(id);
            return fx_modules;
        }(st.fx_modules);
    }
}

template <audio::bus_direction D>
auto
add_mixer_bus(
        audio_state& st,
        std::string name,
        audio::bus_type type,
        channel_index_pair const& chs) -> mixer::bus_id
{
    mixer::bus_id bus_id;
    std::tie(st.mixer_state.buses, bus_id) = [&name, type, chs](
                                                     mixer::buses_t buses,
                                                     parameter_maps& params) {
        auto bus_id = buses.add(mixer::bus{
                .name = std::move(name),
                .volume = params.add(parameter::float_{
                        .default_value = 1.f,
                        .min = 0.f,
                        .max = 4.f}),
                .pan_balance = params.add(parameter::float_{
                        .default_value = 0.f,
                        .min = -1.f,
                        .max = 1.f}),
                .mute = params.add(parameter::bool_{.default_value = false}),
                .level = params.add(parameter::stereo_level{}),
                .type = type,
                .device_channels = chs,
                .fx_chain = {}});

        return std::pair(std::move(buses), bus_id);
    }(st.mixer_state.buses, st.params);

    mixer::bus_list_t bus_ids = mixer::bus_ids<D>(st.mixer_state);
    bus_ids.emplace_back(bus_id);
    mixer::bus_ids<D>(st.mixer_state) = std::move(bus_ids);

    return bus_id;
}

template auto add_mixer_bus<audio::bus_direction::input>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&) -> mixer::bus_id;
template auto add_mixer_bus<audio::bus_direction::output>(
        audio_state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&) -> mixer::bus_id;

void
remove_mixer_bus(audio_state& st, mixer::bus_id const bus_id)
{
    if (st.mixer_state.input_solo_id == bus_id)
        st.mixer_state.input_solo_id = mixer::bus_id{};

    mixer::bus const* const bus = st.mixer_state.buses.get()[bus_id];
    BOOST_ASSERT(bus);
    st.params.remove(bus->volume);
    st.params.remove(bus->pan_balance);
    st.params.remove(bus->mute);
    st.params.remove(bus->level);

    auto const fx_chain = *bus->fx_chain;
    for (auto&& fx_mod_id : fx_chain)
        remove_fx_module(st, fx_mod_id);

    if (st.fx_chain_bus == bus_id)
        st.fx_chain_bus = {};

    if (algorithm::contains(*st.mixer_state.inputs, bus_id))
    {
        auto bus_ids = *st.mixer_state.inputs;
        boost::remove_erase(bus_ids, bus_id);
        st.mixer_state.inputs = std::move(bus_ids);
    }
    else
    {
        auto bus_ids = *st.mixer_state.outputs;
        boost::remove_erase(bus_ids, bus_id);
        st.mixer_state.outputs = std::move(bus_ids);
    }

    st.mixer_state.buses = [bus_id](mixer::buses_t buses) {
        buses.remove(bus_id);
        return buses;
    }(st.mixer_state.buses);
}

template <audio::bus_direction D>
void
clear_mixer_buses(audio_state& st)
{
    auto const bus_ids = mixer::bus_ids<D>(st.mixer_state);
    for (auto const bus_id : *bus_ids)
        remove_mixer_bus(st, bus_id);
}

template void clear_mixer_buses<audio::bus_direction::input>(audio_state&);
template void clear_mixer_buses<audio::bus_direction::output>(audio_state&);

} // namespace piejam::runtime
