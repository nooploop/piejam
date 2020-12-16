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

#include <piejam/runtime/persistence/access.h>

#include <piejam/entity_id_hash.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/locations.h>
#include <piejam/runtime/persistence/app_config.h>
#include <piejam/runtime/persistence/session.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <filesystem>

namespace piejam::runtime::persistence
{

static auto
config_file_path(locations const& locs)
{
    BOOST_ASSERT(!locs.config_dir.empty());

    if (!std::filesystem::exists(locs.config_dir))
        std::filesystem::create_directories(locs.config_dir);

    return locs.config_dir / "piejam.config";
}

void
load_app_config(locations const& locs, dispatch_f const& dispatch)
{
    try
    {
        actions::apply_app_config action;
        action.conf = persistence::load_app_config(config_file_path(locs));
        dispatch(action);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not load config file: {}", err.what());
    }
}

void
save_app_config(locations const& locs, state const& state)
{
    try
    {
        persistence::app_config conf;

        conf.input_device_name =
                state.pcm_devices->inputs[state.input.index].name;
        conf.output_device_name =
                state.pcm_devices->outputs[state.output.index].name;
        conf.samplerate = state.samplerate;
        conf.period_size = state.period_size;

        auto const buses_to_bus_configs = [](auto const& chs,
                                             auto const& ch_ids,
                                             auto& configs) {
            configs.reserve(ch_ids.size());
            std::ranges::transform(
                    ch_ids,
                    std::back_inserter(configs),
                    [&chs](mixer::bus_id const& ch_id)
                            -> persistence::bus_config {
                        mixer::bus const* bus = chs[ch_id];
                        return {bus->name, bus->type, bus->device_channels};
                    });
        };

        buses_to_bus_configs(
                state.mixer_state.buses.get(),
                state.mixer_state.inputs.get(),
                conf.input_bus_config);

        buses_to_bus_configs(
                state.mixer_state.buses.get(),
                state.mixer_state.outputs.get(),
                conf.output_bus_config);

        persistence::save_app_config(conf, config_file_path(locs));
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not save config file: {}", err.what());
    }
}

static auto
export_fx_chains(
        audio_state const& st,
        mixer::bus_list_t const& bus_ids,
        fx::instance_plugin_id_map const& plugin_ids)
        -> std::vector<persistence::session::fx_chain_data>
{
    std::vector<persistence::session::fx_chain_data> result;

    for (auto const& bus_id : bus_ids)
    {
        mixer::bus const* const bus = (*st.mixer_state.buses)[bus_id];

        auto& fx_chain_data = result.emplace_back();

        for (auto const& fx_mod_id : *bus->fx_chain)
        {
            fx::module const* const fx_mod = (*st.fx_modules)[fx_mod_id];
            fx_chain_data.emplace_back(std::visit(
                    overload{
                            [](fx::internal id) -> persistence::fx_plugin_id {
                                return id;
                            },
                            [&plugin_ids](fx::ladspa_instance_id id)
                                    -> persistence::fx_plugin_id {
                                return plugin_ids.at(id);
                            }},
                    fx_mod->fx_instance_id));
        }
    }

    return result;
}

void
load_session(std::filesystem::path const& file, dispatch_f const& dispatch)
{
    actions::apply_session action;
    action.ses = persistence::load_session(file);
    dispatch(action);
}

void
save_session(
        std::filesystem::path const& file,
        state const& st,
        fx::instance_plugin_id_map const& plugin_ids)
{
    session ses;

    ses.inputs = export_fx_chains(st, *st.mixer_state.inputs, plugin_ids);
    ses.outputs = export_fx_chains(st, *st.mixer_state.outputs, plugin_ids);

    save_session(ses, file);
}

} // namespace piejam::runtime::persistence
