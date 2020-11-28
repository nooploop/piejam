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

#include <piejam/runtime/config_access.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/redux/store.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/app_config.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/locations.h>
#include <piejam/runtime/store_dispatch.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <filesystem>

namespace piejam::runtime::config_access
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
load(locations const& locs, store_dispatch dispatch)
{
    try
    {
        actions::apply_app_config action;
        action.conf = load_app_config(config_file_path(locs));
        dispatch(action);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not load config file: {}", err.what());
    }
}

void
save(locations const& locs, state const& state)
{
    try
    {
        app_config conf;

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
                    [&chs](mixer::bus_id const& ch_id) -> bus_config {
                        mixer::bus const* bus = chs[ch_id];
                        return {bus->name, bus->type, bus->device_channels};
                    });
        };

        buses_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.inputs.get(),
                conf.input_bus_config);

        buses_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.outputs.get(),
                conf.output_bus_config);

        runtime::save_app_config(conf, config_file_path(locs));
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not save config file: {}", err.what());
    }
}

} // namespace piejam::runtime::config_access
