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

#include <piejam/runtime/state.h>

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
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::samplerates_t
{
    return set_intersection(
            input_hw_params->samplerates,
            output_hw_params->samplerates);
}

auto
samplerates_from_state(state const& state) -> audio::samplerates_t
{
    return samplerates(state.input.hw_params, state.output.hw_params);
}

auto
period_sizes(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::period_sizes_t
{
    return set_intersection(
            input_hw_params->period_sizes,
            output_hw_params->period_sizes);
}

auto
period_sizes_from_state(state const& state) -> audio::period_sizes_t
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

auto
insert_internal_fx_module(
        state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::internal const fx_type,
        std::vector<fx::parameter_assignment> const& initial_assignments)
        -> fx::module_id
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    fx::chain_t fx_chain = st.mixer_state.buses[bus_id]->fx_chain;
    auto const insert_pos = std::min(position, fx_chain.size());

    fx::parameters_t fx_params = st.fx_parameters;

    fx::module_id fx_mod_id;
    switch (fx_type)
    {
        case fx::internal::gain:
            fx_mod_id = make_fx_gain(st.fx_modules, fx_params, st.params);
            fx_chain.emplace(
                    std::next(fx_chain.begin(), insert_pos),
                    fx_mod_id);
            break;
    }

    st.fx_parameters = std::move(fx_params);

    apply_parameter_assignments(
            initial_assignments,
            *st.fx_modules[fx_chain[insert_pos]],
            st.params);

    st.mixer_state.buses.update(bus_id, [&](mixer::bus& bus) {
        bus.fx_chain = std::move(fx_chain);
    });

    return fx_mod_id;
}

void
insert_ladspa_fx_module(
        state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::ladspa_instance_id const instance_id,
        audio::ladspa::plugin_descriptor const& plugin_desc,
        std::span<audio::ladspa::port_descriptor const> const& control_inputs,
        std::vector<fx::parameter_assignment> const& initial_assignments)
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    fx::chain_t fx_chain = st.mixer_state.buses[bus_id]->fx_chain;
    auto const insert_pos = std::min(position, fx_chain.size());

    fx::parameters_t fx_params = st.fx_parameters;

    fx_chain.emplace(
            std::next(fx_chain.begin(), insert_pos),
            st.fx_modules.add(fx::make_ladspa_module(
                    instance_id,
                    plugin_desc.name,
                    control_inputs,
                    fx_params,
                    st.params.get_map<float_parameter>(),
                    st.params.get_map<int_parameter>(),
                    st.params.get_map<bool_parameter>())));

    st.fx_parameters = std::move(fx_params);

    apply_parameter_assignments(
            initial_assignments,
            *st.fx_modules[fx_chain[insert_pos]],
            st.params);

    st.mixer_state.buses.update(bus_id, [&](mixer::bus& bus) {
        bus.fx_chain = std::move(fx_chain);
    });

    st.fx_ladspa_instances.update(
            [&](fx::ladspa_instances& fx_ladspa_instances) {
                fx_ladspa_instances.emplace(instance_id, plugin_desc);
            });
}

void
insert_missing_ladspa_fx_module(
        state& st,
        mixer::bus_id const bus_id,
        std::size_t const position,
        fx::unavailable_ladspa const& unavail,
        std::string_view const& name)
{
    BOOST_ASSERT(bus_id != mixer::bus_id{});

    auto id = st.fx_unavailable_ladspa_plugins.add(unavail);

    fx::chain_t fx_chain = st.mixer_state.buses[bus_id]->fx_chain;
    auto const insert_pos = std::min(position, fx_chain.size());

    fx_chain.emplace(
            std::next(fx_chain.begin(), insert_pos),
            st.fx_modules.add(fx::module{
                    .fx_instance_id = id,
                    .name = name,
                    .parameters = {}}));

    st.mixer_state.buses.update(bus_id, [&](mixer::bus& bus) {
        bus.fx_chain = std::move(fx_chain);
    });
}

void
remove_fx_module(state& st, fx::module_id id)
{
    if (fx::module const* const fx_mod = st.fx_modules[id])
    {
        for (auto&& [bus_id, bus] : st.mixer_state.buses)
        {
            if (algorithm::contains(*bus.fx_chain, id))
            {
                st.mixer_state.buses.update(bus_id, [id](mixer::bus& bus) {
                    fx::chain_t fx_chain = *bus.fx_chain;
                    boost::remove_erase(fx_chain, id);
                    bus.fx_chain = std::move(fx_chain);
                });
            }
        }

        fx::parameters_t fx_params = st.fx_parameters;
        for (auto&& [key, fx_param_id] : *fx_mod->parameters)
        {
            std::visit([&st](auto&& id) { st.params.remove(id); }, fx_param_id);
            fx_params.erase(fx_param_id);
        }
        st.fx_parameters = std::move(fx_params);

        st.fx_modules.remove(id);

        if (auto id = std::get_if<fx::ladspa_instance_id>(
                    &fx_mod->fx_instance_id))
        {
            st.fx_ladspa_instances.update(
                    [id](fx::ladspa_instances& instances) {
                        instances.erase(*id);
                    });
        }
        else if (
                auto id = std::get_if<fx::unavailable_ladspa_id>(
                        &fx_mod->fx_instance_id))
        {
            st.fx_unavailable_ladspa_plugins.remove(*id);
        }
    }
}

template <io_direction D>
auto
add_mixer_bus(
        state& st,
        std::string name,
        audio::bus_type type,
        channel_index_pair const& chs) -> mixer::bus_id
{
    auto& params = st.params;
    mixer::bus_id const bus_id = st.mixer_state.buses.add(mixer::bus{
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

    mixer::bus_ids<D>(st.mixer_state)
            .update([bus_id](mixer::bus_list_t& bus_ids) {
                bus_ids.emplace_back(bus_id);
            });

    return bus_id;
}

template auto add_mixer_bus<io_direction::input>(
        state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&) -> mixer::bus_id;
template auto add_mixer_bus<io_direction::output>(
        state&,
        std::string,
        audio::bus_type,
        channel_index_pair const&) -> mixer::bus_id;

void
remove_mixer_bus(state& st, mixer::bus_id const bus_id)
{
    if (st.mixer_state.input_solo_id == bus_id)
        st.mixer_state.input_solo_id = mixer::bus_id{};

    mixer::bus const* const bus = st.mixer_state.buses[bus_id];
    BOOST_ASSERT(bus);
    st.params.remove(bus->volume);
    st.params.remove(bus->pan_balance);
    st.params.remove(bus->mute);
    st.params.remove(bus->level);

    fx::chain_t const fx_chain = *bus->fx_chain;
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

    st.mixer_state.buses.remove(bus_id);
}

template <io_direction D>
void
clear_mixer_buses(state& st)
{
    auto const bus_ids = mixer::bus_ids<D>(st.mixer_state);
    for (auto const bus_id : *bus_ids)
        remove_mixer_bus(st, bus_id);
}

template void clear_mixer_buses<io_direction::input>(state&);
template void clear_mixer_buses<io_direction::output>(state&);

} // namespace piejam::runtime