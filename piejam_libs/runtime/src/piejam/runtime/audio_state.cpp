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

static void
apply_parameter_assignments(
        std::vector<fx::parameter_assignment> const& assignments,
        fx::module const& fx_mod,
        parameter_maps& params)
{
    for (auto&& [key, value] : assignments)
    {
        if (auto it = fx_mod.parameters->find(key);
            it != fx_mod.parameters->end())
        {
            auto const& fx_param_id = it->second;
            std::visit(
                    overload{
                            [&params](float_parameter_id id, float v) {
                                params.set(id, v);
                            },
                            [&params](int_parameter_id id, int v) {
                                params.set(id, v);
                            },
                            [&params](bool_parameter_id id, bool v) {
                                params.set(id, v);
                            },
                            [](auto&&, auto&&) { BOOST_ASSERT(false); }},
                    fx_param_id,
                    value);
        }
    }
}

void
insert_internal_fx_module(
        audio_state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::internal const fx_type,
        std::vector<fx::parameter_assignment> const& initial_assignments)
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    std::tie(st.mixer_state.buses, st.fx_modules, st.fx_parameters) =
            [bus_id, position, fx_type, &initial_assignments](
                    mixer::buses_t buses,
                    fx::chain_t fx_chain,
                    fx::modules_t fx_modules,
                    fx::parameters_t fx_params,
                    parameter_maps& params) {
                mixer::bus& bus = buses[bus_id];

                auto const insert_pos = std::min(position, fx_chain.size());

                switch (fx_type)
                {
                    case fx::internal::gain:
                        fx_chain.emplace(
                                std::next(fx_chain.begin(), insert_pos),
                                make_fx_gain(fx_modules, fx_params, params));
                        break;
                }

                apply_parameter_assignments(
                        initial_assignments,
                        *std::as_const(fx_modules)[fx_chain[insert_pos]],
                        params);

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
insert_ladspa_fx_module(
        audio_state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::ladspa_instance_id const instance_id,
        audio::ladspa::plugin_descriptor const& plugin_desc,
        std::span<audio::ladspa::port_descriptor const> const& control_inputs,
        std::vector<fx::parameter_assignment> const& initial_assignments)
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    std::tie(
            st.mixer_state.buses,
            st.fx_modules,
            st.fx_parameters,
            st.fx_ladspa_instances) =
            [bus_id,
             position,
             instance_id,
             &plugin_desc,
             control_inputs,
             &initial_assignments](
                    mixer::buses_t buses,
                    fx::chain_t fx_chain,
                    fx::modules_t fx_modules,
                    fx::parameters_t fx_params,
                    fx::ladspa_instances fx_ladspa_instances,
                    parameter_maps& params) {
                mixer::bus& bus = buses[bus_id];

                auto const insert_pos = std::min(position, fx_chain.size());

                fx_chain.emplace(
                        std::next(fx_chain.begin(), insert_pos),
                        fx_modules.add(fx::make_ladspa_module(
                                instance_id,
                                plugin_desc.name,
                                control_inputs,
                                fx_params,
                                params.get_map<float_parameter>(),
                                params.get_map<int_parameter>(),
                                params.get_map<bool_parameter>())));

                apply_parameter_assignments(
                        initial_assignments,
                        *std::as_const(fx_modules)[fx_chain[insert_pos]],
                        params);

                bus.fx_chain = std::move(fx_chain);

                fx_ladspa_instances.emplace(instance_id, plugin_desc);

                return std::tuple(
                        std::move(buses),
                        std::move(fx_modules),
                        std::move(fx_params),
                        std::move(fx_ladspa_instances));
            }(st.mixer_state.buses,
              st.mixer_state.buses.get()[bus_id]->fx_chain,
              st.fx_modules,
              st.fx_parameters,
              st.fx_ladspa_instances,
              st.params);
}

void
insert_missing_ladspa_fx_module(
        audio_state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::unavailable_ladspa const& unavail,
        std::string_view const& name)
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    std::tie(
            st.mixer_state.buses,
            st.fx_modules,
            st.fx_unavailable_ladspa_plugins) =
            [bus_id, position, &unavail, name](
                    mixer::buses_t buses,
                    fx::chain_t fx_chain,
                    fx::modules_t fx_modules,
                    fx::unavailable_ladspa_plugins unavail_plugs) {
                mixer::bus& bus = buses[bus_id];

                auto const insert_pos = std::min(position, fx_chain.size());

                auto id = unavail_plugs.add(unavail);

                fx_chain.emplace(
                        std::next(fx_chain.begin(), insert_pos),
                        fx_modules.add(fx::module{
                                .fx_instance_id = id,
                                .name = name,
                                .parameters = {}}));

                bus.fx_chain = std::move(fx_chain);

                return std::tuple(
                        std::move(buses),
                        std::move(fx_modules),
                        std::move(unavail_plugs));
            }(st.mixer_state.buses,
              st.mixer_state.buses.get()[bus_id]->fx_chain,
              st.fx_modules,
              st.fx_unavailable_ladspa_plugins);
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

        if (auto id = std::get_if<fx::ladspa_instance_id>(
                    &fx_mod->fx_instance_id))
        {
            st.fx_ladspa_instances = [id](fx::ladspa_instances instances) {
                instances.erase(*id);
                return instances;
            }(st.fx_ladspa_instances);
        }
        else if (
                auto id = std::get_if<fx::unavailable_ladspa_id>(
                        &fx_mod->fx_instance_id))
        {
            st.fx_unavailable_ladspa_plugins =
                    [id](fx::unavailable_ladspa_plugins unavail_plugs) {
                        unavail_plugs.remove(*id);
                        return unavail_plugs;
                    }(st.fx_unavailable_ladspa_plugins);
        }
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
    auto& params = st.params;
    mixer::bus_id const bus_id = st.mixer_state.buses.update(
            [&params, &name, type, chs](mixer::buses_t& buses) {
                return buses.add(mixer::bus{
                        .name = std::move(name),
                        .volume = params.add(parameter::float_{
                                .default_value = 1.f,
                                .min = 0.f,
                                .max = 4.f}),
                        .pan_balance = params.add(parameter::float_{
                                .default_value = 0.f,
                                .min = -1.f,
                                .max = 1.f}),
                        .mute = params.add(
                                parameter::bool_{.default_value = false}),
                        .level = params.add(parameter::stereo_level{}),
                        .type = type,
                        .device_channels = chs,
                        .fx_chain = {}});
            });

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

    st.mixer_state.buses.update(
            [bus_id](mixer::buses_t& buses) { buses.remove(bus_id); });
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
