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

#include <piejam/runtime/app_config.h>

#include <nlohmann/json.hpp>

#include <fstream>

namespace piejam::runtime
{

using namespace std::string_literals;

static auto const s_key_version = "version";
static auto const s_key_audio_settings = "audio_settings";
static auto const s_key_input_device_name = "input_device_name"s;
static auto const s_key_output_device_name = "output_device_name";
static auto const s_key_samplerate = "samplerate";
static auto const s_key_period_size = "period_size";

auto
to_json(app_config const& conf) -> nlohmann::json
{
    return {{s_key_version, current_app_config_version},
            {s_key_audio_settings,
             {{s_key_input_device_name, conf.input_device_name},
              {s_key_output_device_name, conf.output_device_name},
              {s_key_samplerate, conf.samplerate},
              {s_key_period_size, conf.period_size}}}};
}

auto
from_json(nlohmann::json const& json_conf) -> app_config
{
    app_config conf;
    auto const& audio_settings = json_conf[s_key_audio_settings];
    audio_settings[s_key_input_device_name].get_to(conf.input_device_name);
    audio_settings[s_key_output_device_name].get_to(conf.output_device_name);
    audio_settings[s_key_samplerate].get_to(conf.samplerate);
    audio_settings[s_key_period_size].get_to(conf.period_size);
    return conf;
}

auto
load_app_config(std::filesystem::path const& file) -> app_config
{
    std::ifstream in(file);
    if (!in.is_open())
        throw std::runtime_error("could not open config file");

    auto json_conf = nlohmann::json::parse(in);

    auto file_version = json_conf[s_key_version].get<unsigned>();

    if (file_version > current_app_config_version)
        throw std::runtime_error("app config version is too new");

    if (file_version < current_app_config_version)
        throw std::runtime_error(
                "versioning of the app config file is not implemented yet");

    return from_json(json_conf);
}

void
save_app_config(app_config const& conf, std::filesystem::path const& file)
{
    nlohmann::json json_conf = to_json(conf);

    std::ofstream out(file);
    out << json_conf.dump(4);
}

} // namespace piejam::runtime
