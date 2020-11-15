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

#include <piejam/app/config_access.h>

#include <piejam/algorithm/index_of.h>
#include <piejam/redux/store.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/app_config.h>
#include <piejam/runtime/audio_state.h>

#include <QStandardPaths>

#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>

namespace piejam::app::config_access
{

static auto
config_file_path()
{
    static auto const config_dir = std::filesystem::path(
            QStandardPaths::writableLocation(
                    QStandardPaths::StandardLocation::ConfigLocation)
                    .toStdString());
    static auto const s_file = config_dir / "piejam.config";

    if (!std::filesystem::exists(config_dir))
        std::filesystem::create_directories(config_dir);

    return s_file;
}

void
load(store& st)
{
    using namespace piejam::runtime;

    try
    {
        actions::apply_app_config action;
        action.conf = load_app_config(config_file_path());
        st.dispatch<actions::apply_app_config>(std::move(action));
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not load config file: {}", err.what());
    }
}

void
save(runtime::audio_state const& state)
{
    try
    {
        runtime::app_config conf;

        conf.input_device_name =
                state.pcm_devices->inputs[state.input.index].name;
        conf.output_device_name =
                state.pcm_devices->outputs[state.output.index].name;
        conf.samplerate = state.samplerate;
        conf.period_size = state.period_size;

        auto const channels_to_bus_configs =
                [](auto const& chs, auto const& ch_ids, auto& configs) {
                    configs.reserve(ch_ids.size());
                    std::ranges::transform(
                            ch_ids,
                            std::back_inserter(configs),
                            [&chs](runtime::mixer::bus_id const& ch_id)
                                    -> runtime::bus_config {
                                auto const& ch = chs[ch_id];
                                return {ch.name, ch.type, ch.device_channels};
                            });
                };

        channels_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.inputs,
                conf.input_bus_config);

        channels_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.outputs,
                conf.output_bus_config);

        runtime::save_app_config(conf, config_file_path());
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not save config file: {}", err.what());
    }
}

} // namespace piejam::app::config_access
