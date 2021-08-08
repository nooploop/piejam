// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/access.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/entity_id_hash.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/runtime/persistence/app_config.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace piejam::runtime::persistence
{

void
load_app_config(std::filesystem::path const& file, dispatch_f const& dispatch)
{
    if (!std::filesystem::exists(file))
        return;

    try
    {
        std::ifstream in(file);
        if (!in.is_open())
            throw std::runtime_error("could not open config file");

        actions::apply_app_config action;
        action.conf = persistence::load_app_config(in);
        dispatch(action);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not load config file: {}", err.what());
    }
}

void
save_app_config(
        std::filesystem::path const& file,
        std::vector<std::string> const& enabled_midi_input_devices,
        state const& state)
{
    try
    {
        std::ofstream out(file);
        if (!out.is_open())
            throw std::runtime_error("could not open config file");

        persistence::app_config conf;

        conf.input_device_name =
                state.input.index != npos
                        ? state.pcm_devices->inputs[state.input.index].name
                        : std::string();
        conf.output_device_name =
                state.output.index != npos
                        ? state.pcm_devices->outputs[state.output.index].name
                        : std::string();
        conf.sample_rate = state.sample_rate;
        conf.period_size = state.period_size;
        conf.period_count = state.period_count;

        auto const buses_to_bus_configs =
                [](auto const& chs, auto const& ch_ids, auto& configs) {
                    configs.reserve(ch_ids.size());
                    std::ranges::transform(
                            ch_ids,
                            std::back_inserter(configs),
                            [&chs](device_io::bus_id const& ch_id)
                                    -> persistence::bus_config {
                                device_io::bus const& bus = chs[ch_id];
                                return {bus.name, bus.bus_type, bus.channels};
                            });
                };

        buses_to_bus_configs(
                state.device_io_state.buses,
                state.device_io_state.inputs.get(),
                conf.input_bus_config);

        buses_to_bus_configs(
                state.device_io_state.buses,
                state.device_io_state.outputs.get(),
                conf.output_bus_config);

        conf.enabled_midi_input_devices = enabled_midi_input_devices;

        conf.rec_session = state.rec_session + 1;

        persistence::save_app_config(out, conf);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not save config file: {}", err.what());
    }
}

static auto
export_parameter_values(fx::module const& fx_mod, parameter_maps const& params)
        -> std::vector<fx::parameter_value_assignment>
{
    std::vector<fx::parameter_value_assignment> result;

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit(
                [&](auto&& param) {
                    result.emplace_back(fx::parameter_value_assignment{
                            key,
                            get_parameter_value(params, param)});
                },
                fx_param_id);
    }

    return result;
}

static auto
export_fx_midi_assignments(
        fx::module const& fx_mod,
        midi_assignments_map const& midi_assigns)
        -> std::vector<fx::parameter_midi_assignment>
{
    std::vector<fx::parameter_midi_assignment> result;

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit(
                [&](auto&& param) {
                    if (auto it = midi_assigns.find(param);
                        it != midi_assigns.end())
                    {
                        result.emplace_back(
                                fx::parameter_midi_assignment{key, it->second});
                    }
                },
                fx_param_id);
    }

    return result;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::internal const fx_type) -> session::fx_plugin
{
    BOOST_ASSERT(std::get<fx::internal>(fx_mod.fx_instance_id) == fx_type);

    session::internal_fx fx;
    fx.type = fx_type;
    fx.preset = export_parameter_values(fx_mod, st.params);
    fx.midi = export_fx_midi_assignments(fx_mod, st.midi_assignments);
    return fx;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::ladspa_instance_id const id) -> session::fx_plugin
{
    BOOST_ASSERT(std::get<fx::ladspa_instance_id>(fx_mod.fx_instance_id) == id);

    session::ladspa_plugin plug;
    auto const& pd = st.fx_ladspa_instances->at(id);
    plug.id = pd.id;
    plug.name = pd.name;
    plug.preset = export_parameter_values(fx_mod, st.params);
    plug.midi = export_fx_midi_assignments(fx_mod, st.midi_assignments);
    return plug;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::unavailable_ladspa_id const id) -> session::fx_plugin
{
    BOOST_ASSERT(
            std::get<fx::unavailable_ladspa_id>(fx_mod.fx_instance_id) == id);

    auto const& unavail = st.fx_unavailable_ladspa_plugins[id];
    session::ladspa_plugin plug;
    plug.id = unavail.plugin_id;
    plug.name = fx_mod.name;
    plug.preset = unavail.parameter_values;
    plug.midi = unavail.midi_assignments;
    return plug;
}

static auto
export_fx_chain(state const& st, fx::chain_t const& fx_chain)
        -> persistence::session::fx_chain_t
{
    persistence::session::fx_chain_t result;

    for (auto const& fx_mod_id : fx_chain)
    {
        fx::module const& fx_mod = st.fx_modules[fx_mod_id];
        result.emplace_back(std::visit(
                [&st, &fx_mod](auto const& id) {
                    return export_fx_plugin(st, fx_mod, id);
                },
                fx_mod.fx_instance_id));
    }

    return result;
}

static auto
export_mixer_midi(state const& st, mixer::channel const& bus)
        -> persistence::session::mixer_midi
{
    persistence::session::mixer_midi result;

    if (auto it = st.midi_assignments->find(bus.volume);
        it != st.midi_assignments->end())
        result.volume = it->second;

    if (auto it = st.midi_assignments->find(bus.pan_balance);
        it != st.midi_assignments->end())
        result.pan = it->second;

    if (auto it = st.midi_assignments->find(bus.mute);
        it != st.midi_assignments->end())
        result.mute = it->second;

    return result;
}

static auto
export_mixer_parameters(state const& st, mixer::channel const& bus)
{
    session::mixer_parameters result;
    result.volume = get_parameter_value(st.params, bus.volume);
    result.pan = get_parameter_value(st.params, bus.pan_balance);
    result.mute = get_parameter_value(st.params, bus.mute);
    return result;
}

static auto
export_mixer_io(state const& st, mixer::io_address_t const& addr)
{
    return std::visit(
            boost::hof::match(
                    [](std::nullptr_t) {
                        return session::mixer_io{
                                .type = session::mixer_io_type::default_,
                                .name = {}};
                    },
                    [&st](mixer::channel_id const& channel_id) {
                        mixer::channel const& channel =
                                st.mixer_state.channels[channel_id];
                        return session::mixer_io{
                                .type = session::mixer_io_type::channel,
                                .name = channel.name};
                    },
                    [&st](device_io::bus_id const& bus_id) {
                        device_io::bus const& bus =
                                st.device_io_state.buses[bus_id];
                        return session::mixer_io{
                                .type = session::mixer_io_type::device,
                                .name = bus.name};
                    },
                    [](mixer::missing_device_address const& name) {
                        return session::mixer_io{
                                .type = session::mixer_io_type::device,
                                .name = name};
                    }),
            addr);
}

static auto
export_mixer_channel(state const& st, mixer::channel const& bus)
{
    session::mixer_channel result;
    result.name = bus.name;
    result.fx_chain = export_fx_chain(st, *bus.fx_chain);
    result.midi = export_mixer_midi(st, bus);
    result.parameter = export_mixer_parameters(st, bus);
    result.in = export_mixer_io(st, bus.in);
    result.out = export_mixer_io(st, bus.out);
    return result;
}

static auto
export_mixer_channels(state const& st, mixer::channel_ids_t const& channel_ids)
{
    return algorithm::transform_to_vector(
            channel_ids,
            [&st](mixer::channel_id const channel_id) {
                mixer::channel const& bus = st.mixer_state.channels[channel_id];
                return export_mixer_channel(st, bus);
            });
}

void
load_session(std::filesystem::path const& file, dispatch_f const& dispatch)
{
    if (!std::filesystem::exists(file))
        return;

    try
    {
        std::ifstream in(file);
        if (!in.is_open())
            throw std::runtime_error("could not open session file");

        actions::apply_session(persistence::load_session(in), dispatch);
    }
    catch (std::exception const& err)
    {
        spdlog::error("load_session: {}", err.what());
    }
}

void
save_session(std::filesystem::path const& file, state const& st)
{
    try
    {
        std::ofstream out(file);
        if (!out.is_open())
            throw std::runtime_error("could not open session file");

        session ses;

        ses.mixer_channels = export_mixer_channels(st, *st.mixer_state.inputs);
        ses.main_mixer_channel = export_mixer_channel(
                st,
                st.mixer_state.channels[st.mixer_state.main]);

        save_session(out, ses);
    }
    catch (std::exception const& err)
    {
        spdlog::error("save_session: {}", err.what());
    }
}

} // namespace piejam::runtime::persistence
